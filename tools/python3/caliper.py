


def parse_tab_hier(lines):

    for line in lines:
        print(line.strip("\n"))

def parse_tab_hier_string(string):
    print("parse_tab_hier_string")

def parse_tab_hier_file(filepath):
    print("parse_tab_hier_file", filepath)
    with open(filepath) as file:
        parse_tab_hier(file.readlines())
    return 1
