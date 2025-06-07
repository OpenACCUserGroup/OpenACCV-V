class tag_evaluation:
    def __init__(self, tag_string):
        self.tag_order = get_all_tags(tag_string)
        self.lookup = self.recursive_build(self.tag_order, [], tag_string)

    def eval_fast(self, tags):
        temp_reference = self.lookup
        for x in self.tag_order:
            if x in tags:
                temp_reference = temp_reference[True]
            else:
                temp_reference = temp_reference[False]
        return temp_reference

    def recursive_build(self, remaining_tags, values, tag_string):
        if len(remaining_tags) == 0:
            return self.eval_slow(values, tag_string)
        else:
            lookup = {False: self.recursive_build(remaining_tags[1:], values + [False], tag_string),
                      True: self.recursive_build(remaining_tags[1:], values + [True], tag_string)}
            return lookup

    def eval_slow(self, values, tag_string):
        depth = 0
        passed_str = ""
        start = 0
        for x in list(range(len(tag_string))):
            if tag_string[x] == "(":
                if depth > 0:
                    passed_str = passed_str + tag_string[x]
                else:
                    start = x
                depth += 1
            elif tag_string[x] == ")":
                if depth == 1:
                    tag_string = tag_string[:start] + str(self.eval_slow(values, passed_str)) + tag_string[x + 1:]
                    passed_str = ""
                    depth -= 1
                depth -= 1
            else:
                if depth > 0:
                    passed_str = passed_str + tag_string[x]
        internal_tag_order = []
        internal_values = []
        for x in self.tag_order:
            internal_tag_order.append(x)
        for x in values:
            internal_values.append(x)
        for x in list(range(len(self.tag_order))):
            internal_tag_order.append("!" + self.tag_order[x])
            internal_values.append(not values[x])
        internal_tag_order.append("!False")
        internal_values.append(True)
        internal_tag_order.append("!True")
        internal_values.append(False)
        ops = []
        for x in list(range(len(internal_tag_order))):
            tag_string = tag_string.replace(internal_tag_order[x], str(internal_values[x]))
        for x in tag_string:
            if x == "|":
                ops.append("|")
            elif x == "&":
                ops.append("&")
        tag_string = tag_string.replace("|", "&")
        bools = tag_string.split("&")
        result = None
        if len(ops) == 0:
            return bo(bools[0])
        if ops[0] == "|":
            result = bo(bools[0]) or bo(bools[1])
        if ops[0] == "&":
            result = bo(bools[0]) and bo(bools[1])
        if len(ops) == 1:
            return result
        for x in list(range(1, len(ops))):
            if ops[x] == "|":
                result = result or bools[x + 1]
            if ops[x] == "&":
                result = result and bools[x + 1]
        return result