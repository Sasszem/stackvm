import subprocess
# We use subprocess to call cpp - the C preprocessor
from array import array


class Compiler(object):

    opcodes = {}
    # Opcodes holds the named opcodes as defined in opcodes.hpp

    
    def __init__(self):
        self.read_opcodes()
        self.make_commands()

    # Watch out!
    # make_commands() replaces the values in this dict
    # with the values defined in opcodes.hpp
    commands = {
                # Stack manipulation
                "push":     "OP_PUSH",
                "drop":     "OP_DROP",
                "swap":     "OP_SWAP",
                "dup":      "OP_DUP",
                "rot":      "OP_ROT",

                # basic arithmetic
                "+":        "OP_ADD",
                "-":        "OP_SUB",
                "*":        "OP_MUL",
                "/":        "OP_DIV",
                "%":        "OP_MOD",

                # comparing values
                "=":        "OP_EQ",
                ">":        "OP_GT",
                "<":        "OP_LT",

                # RAM manipulation
                "store":    "OP_STOR",
                "load":     "OP_LOAD",

                # control flow
                "goto":     "OP_GOTO",
                "jz":       "OP_JZ",
                "call":     "OP_CALL",
                "ret":      "OP_RET",

                # other
                "halt":     "OP_HALT",
                }

    def read_opcodes(self, filename="./src/opcodes.hpp"):
        "Read the opcode definitions from the definitions file"
        with open(filename) as f:
            # Read the lines, strip and split them (by spaces)
            all_lines = map(str.split, map(str.strip, f.read().split("\n")))
            for line in all_lines:
                # Sry about theese ifs
                if line:
                    if line[0]=="#define":
                        if line[1].startswith("OP_"):
                            self.opcodes[line[1]]=int(line[2])
                            # #define OP_whatever some_number
                            #    0        1           2

    def make_commands(self):
        "Replace the values of self.commands with the corresponding opcode values"
        for cmd in self.commands:
            key = self.commands[cmd]
            opcode = self.opcodes[key]
            self.commands[cmd]=opcode

    def tokenize(self,data):
        "Break the source into tokens"
        return data.split()

    def compile(self,tokens):
        "Compile a sequence of tokens"

        # Null those on each file
        # This way we can compile multiple files with the same instance
        self.label_locations = {}
        self.label_definitions = {}

        result = []
        # The length so far
        # (some commands don't produce any code in the binary)
        l = 0
        
        for token in tokens:
            res = self.process_single_token(token, l)
            l += len(res)
            result += res
            
        # Result now only has placeholders instead of labels
        with_labels = self.place_label_values(result)

        
        return with_labels
        
    def preprocess(self,data):
        "Call cpp - the C preprocessor on data"
        cpp = subprocess.Popen(["cpp","-P"],stdin=subprocess.PIPE,stdout=subprocess.PIPE)
        out, err = cpp.communicate(data)
        cpp.wait()
        return out

    def is_number(self, token):
        "Test if a token is a number"
        try:
            num = int(token)
        except ValueError:
            return False
        return True
    def is_label_definition(self, token):
        "Test if a token is a label definition"
        return token.endswith(":")
    def is_label(self, token):
        "Test if a token is a label"
        return token.startswith("&")
    def is_string(self, token):
        "Test if a token is a string"
        return token.startswith("\"") and token.endswith("\"")

    def process_single_token(self, token, l):
        "Compile a single token and return the result"
        
        if self.is_label_definition(token):
            self.define_label(token, l)
            return []

        elif self.is_label(token):
            self.add_label_location(token, l)
            # Return placeholder
            # Real values get substituted later
            return [-1111]

        elif self.is_number(token):
            return [int(token)]

        elif self.is_string(token):
            return array("B",token).tolist()

        else:
            return [self.commands[token]]
            # TODO: Print some fancier error if no such command

    def place_label_values(self, data):
        "Substitute label placeholders with real values"

        # Each label
        for label in self.label_definitions:
            # the value to substitute in
            value = self.label_definitions[label]
    
            # do it on each location
            for loc in self.label_locations[label]:
                data[loc]=value

        return data
    def add_label_location(self, token, l):
        "Add a location of a placeholder to later substitute in label"
        
        label = token[1:] # cut off '&' from the start

        # enable forward-referencing
        if label not in self.label_locations:
            self.label_locations[label]=[]

        self.label_locations[label].append(l)
    def define_label(self, token, loc):
        "Define a label at location loc"
        
        label = token[:-1] # cut off the ':' from the end

        # Multiple definitions are not allowed
        if label in self.label_definitions:
            raise Exception("Label {} is already defined!".format(label))
        
        self.label_definitions[label]=loc
        if not label in self.label_locations:
            self.label_locations[label]=[]
    
    def make_binary(self, data):
        "Translate a sequence of number to a sequence of bytes"
        return array("h",data).tostring()
        
    def read_file(self, filename):
        "Read the contents of a file"
        with open(filename) as f:
            return f.read()
    
    def compile_file(self, filename, output="a.out"):
        "Fully compile a file and write the output"

        # Raw input
        raw = self.read_file(filename)

        # Run cpp on it
        preprocessed = self.preprocess(raw)

        # Tokenize
        tokens = self.tokenize(preprocessed)

        # Compile into a sequence of numbers
        compiled = self.compile(tokens)

        # Make a short[] from it
        binary = self.make_binary(compiled)
        
        # Write it to file
        with open(output,"wb") as out:
            out.write(binary)
        
        print("Succesfuly compiled file {} to {}".format(filename,output))
if __name__=='__main__':

    # Simple CLI with argparser
    import argparse
    
    parser = argparse.ArgumentParser()

    parser.add_argument('inputfile',help="the input file to process")
    parser.add_argument('outputfile',default="a.out", help="the file to write the compiled binary")

    options = parser.parse_args()

    Compiler().compile_file(options.inputfile,options.outputfile)
