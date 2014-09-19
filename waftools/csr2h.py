from waflib.Task import Task
from waflib.TaskGen import extension


class csr2h(Task):
    run_str = 'vsrd ${SRC} --static -o ${TGT}'
    ext_in = '.csr'
    ext_out = '.h'

    def scan(self):
        import re
        filename = re.compile('@.* : (.*);')
        ret = []
        for source in self.inputs:
            f = source.read()
            for line in f.split('\n'):
                result = filename.match(line)
                if result is not None:
                    ret.append(source.parent.find_node(result.group(1)))
        return (ret, ())


@extension('.csr')
def process_csr(self, node):
    self.create_task('csr2h', node, node.change_ext('.h'))
