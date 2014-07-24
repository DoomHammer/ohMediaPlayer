from ci import (OpenHomeBuilder, require_version)

require_version(34)


class Builder(OpenHomeBuilder):

    def setup(self):
        self.env.update(
            WAFLOCK='.lock-wafbuild',
            )
        self.configure_args = self.get_dependency_args(env={'debugmode':self.configuration})
        self.configure_args += ["--" + self.configuration.lower()]

    def configure(self):
        self.python("waf", "configure", *self.configure_args)

    def clean(self):
        self.python('waf', 'clean')

    def build(self):
        from glob import glob
        import os
        path = ':'.join(map(lambda x: os.path.join(os.getcwd(), x), glob('dependencies/*/*')))
        self.env.update(
            PATH='%s:%s' % (self.env['PATH'], path),
            )
        self.python('waf')

    def test(self):
        #self.python("waf", "test")
        pass


    def publish(self):
        # Publish native packages regardless of platform
        pass
