



class Node:
    def __init__(self, key, parent=None, **kwargs):
        self.key    = key
        self.kids   = []
        self.parent = None
        self.kwargs = {}
        for k, v in kwargs.items():
            self.kwargs[k] = v

    def print(self, indent=0):
        print("node", node.key, node.kwargs)
        for kid in node.kids:
            kid.print(kid, indent+1)

def parse_key(key, parent):
    return f"{parent.key}_{key.strip('_')}".lstrip('_')

def parse_node(line, parent):
    while '  ' in line:
        line = line.replace('  ', ' ')
    bits = line.split(' ')
    return Node(parse_key(bits[0], parent), parent, min=bits[1], max=bits[2], avg=bits[3], time=bits[4])


def parse_tab_hier(lines):
    prev_parents = {}
    def parse(prev_indent, line_idx, prev_node):
        if line_idx >= len(lines):
            return
        line = lines[line_idx]
        indent = len(line) - len(line.lstrip())
        if indent > prev_indent:
            line_node = parse_node(line.lstrip(), prev_node)
            prev_parents[indent] = prev_node
            prev_node.kids += [line_node]
            parse(indent, line_idx+1, line_node)
        if indent == prev_indent:
            parent = prev_parents[indent]
            line_node = parse_node(line.lstrip(), parent)
            prev_parents[indent].kids += [line_node]
            parse(indent, line_idx+1, line_node)
        if indent < prev_indent:
            parent = prev_parents[indent]
            line_node = parse_node(line.lstrip(), parent)
            parent.kids += [line_node]
            parse(indent, line_idx+1, line_node)

    prev_parents["root"] = Node("")
    parse(-1, 1, prev_parents["root"])
    return prev_parents["root"]

def parse_tab_hier_file(filepath):
    with open(filepath) as file:
        return parse_tab_hier(file.readlines())
