#!/usr/bin/python

import subprocess
import sys
import re
import os
import os.path

gccinfo = subprocess.Popen(["gcc", "--version"], stdout=subprocess.PIPE, stderr = subprocess.STDOUT).communicate()[0]
#res = re.match("\s+(\d+[.]*\d+)[.]?\d+$", gccinfo)
res = re.search(r"(\d+[.]*\d+[.]?\d+)\n", gccinfo)
GCCVERSION = 'gcc-%s' % res.group(1)

def grep(filename, ematch):
    result = None
    try:
        f_in = open(filename, 'r')
    except IOError:
        pass
    else:
        with f_in:
            result = [line for line in f_in if re.search(ematch, line)]
    return result

def bjam_gcc():
    # get gcc version used in bjam config
    result = None
    # bjam_user_config_path = "~/user-config.jam"
    # print grep(bjam_user_config_path, r'^using gcc')
    bjam_site_config_path = "/etc/site-config.jam"
    matches = grep(bjam_site_config_path, r'^using gcc')
    if matches:
        for line in matches:
            res = re.search(r'^using gcc : (\d+[.]*\d+)', matches[0])
            if res:
                result = "gcc-%s" % res.group(1)
                break
    return result

BJAMGCC = bjam_gcc()
if BJAMGCC:
    GCCVERSION = BJAMGCC

TESTSSUBDIR = ''
if GCCVERSION[:9] in ['gcc-4.6.1']:
    GCCVERSION = GCCVERSION[:9]
    TESTSSUBDIR = 'tests/'
elif GCCVERSION[:7] in ['gcc-4.6', 'gcc-4.7', 'gcc-4.8']:
    GCCVERSION = GCCVERSION[:7]
    TESTSSUBDIR = 'tests/'

print GCCVERSION, TESTSSUBDIR


class Function:
    def __init__(self, name, startline):
        self.name = name
        self.startline = startline
        self.total_lines = 0
        self.covered_lines = 0

class Module:
    def __init__(self, name):
        self.name = name
        self.extension = '.hpp'
        if '/rio/' in self.name:
            self.extension = '.h'
        self.functions = {}

    def total_functions(self):
        len(self.functions)


def list_modules():
    for line in open("./tools/coverage.reference"):
        res = re.match(r'^((?:[/A-Za-z0-9_]+[/])*([/A-Za-z0-9_]+))\s+(\d+)\s+(\d+)', line)
        if res:
            module, name = res.group(1, 2)
            extension = '.hpp'
            if '/rio/' in module:
                extension = '.h'
            yield module, name, extension
        else:
            if line[0] != '\n' and line[0] != '#':
                print "Line '%s' does not match in coverage.reference" % line
                sys.exit(-1)

class Cover:
    def __init__(self):
        self.modules = {}
        self.bestcoverage = {} # module: (lincov, lintotal)
        self.functions = {}
        self.verbose = 1
        self.all = False

    def cover(self, module, name, extension):
        print "Computing coverage for %s" % module
        cmd1 = ["bjam", "coverage", "test_%s" % name]
        cmd2 = ["gcov", "--unconditional-branches", "--all-blocks", "--branch-count", "--branch-probabilities", "--function-summaries", "-o", "bin/%s/coverage/%s%stest_%s.gcno" % (GCCVERSION, TESTSSUBDIR, "%s" % module[:-len(name)] if TESTSSUBDIR else '', name), "bin/%s/coverage/test_%s" % (GCCVERSION, name)]
        cmd3 = ["etags", "-o", "coverage/%s/%s%s.TAGS" % (module, name, extension), "%s%s" % (module, extension)]

        res = subprocess.Popen(cmd1, stdout=subprocess.PIPE, stderr = subprocess.STDOUT).communicate()[0]
        res = subprocess.Popen(cmd2, stdout=subprocess.PIPE, stderr = subprocess.STDOUT).communicate()[0]
        subprocess.call("mkdir -p coverage/%s" % module, shell=True)
        res = subprocess.Popen(cmd3, stdout=subprocess.PIPE, stderr = subprocess.STDOUT).communicate()[0]

        try:
            subprocess.check_call("mv *.gcov coverage/%s" % module, shell=True)
        except:
            print "Failed to compute coverage for module"
            print " ".join(cmd1)
            print " ".join(cmd2)
            print " ".join(cmd3)


    def compute_coverage(self, module, name, extension):
        ftags = "./coverage/%s/%s%s.TAGS" % (module, name, extension)
        fgcov = "./coverage/%s/%s%s.gcov" % (module, name, extension)

        import os.path
        if os.path.isfile(fgcov) and os.path.isfile(ftags):

            self.modules[module] = Module(module)
            for line in open(ftags):
                if re.match(r'^.*(TODO|REDOC|BODY)', line):
                    continue
                res = re.match(r'^(.*[(].*)\x7F.*\x01(\d+)[,].*$', line)
                if res is None:
                    res = re.match(r'^(.*[(].*)\x7F(\d+)[,].*$', line)
                if res:
                    name, startline = res.group(1, 2)
                    print "function found at %s %s" % (name, startline)
                    self.modules[module].functions[int(startline)] = Function(name, int(startline))

            current_function = None
            search_begin_body = False
            brackets_count = 0
            parentheses_count = 0
            open_parentheses = False
            block_executed = False
            for line in open(fgcov):

                res = re.match(r'^\s*(#####|[-]|\d+)[:]\s*(\d+)[:](.*)$', line)
                if res:
                    if current_function and search_begin_body:
                        if res.group(3).count('{'):
                            search_begin_body = False
                        else:
                            continue

                    if int(res.group(2)) in self.modules[module].functions:
                        print "current function found at %d" % int(res.group(2))
                        current_function = int(res.group(2))
                        self.modules[module].functions[current_function].total_lines += 1
                        if not res.group(1) in ('#####', "-"):
                            self.modules[module].functions[current_function].covered_lines += 1
                        brackets_count = res.group(3).count('{')
                        open_parentheses = False
                        if brackets_count > 0:
                            brackets_count -= res.group(3).count('}')
                            if brackets_count <= 0:
                                current_function = None
                        else:
                            search_begin_body = True
                        continue

                    if current_function and not search_begin_body:
                        skip_line = False
                        parentheses_count += res.group(3).count('(')
                        parentheses_count -= res.group(3).count(')')
                        if not open_parentheses:
                            if parentheses_count > 0:
                                open_parentheses = True
                                block_executed = False
                                self.modules[module].functions[current_function].total_lines += 1
                        if open_parentheses:
                            if not res.group(1) in ("#####", "-"):
                                block_executed = True
                            if (parentheses_count <= 0):
                                if block_executed:
                                    self.modules[module].functions[current_function].covered_lines += 1
                                open_parentheses = False
                                parentheses_count = 0
                            skip_line = True
                        brackets_count += res.group(3).count('{')
                        brackets_count -= res.group(3).count('}')
                        if brackets_count <= 0:
                            brackets_count = 0
                            current_function = None
                            skip_line = True
                        if skip_line:
                            continue

                    # ignore comments
                    if re.match('^\s*//', res.group(3)):
                        continue
                    # ignore blank lines
                    if re.match('^\s+$', res.group(3)):
                        continue
                    if re.match(r'^.*(TODO|REDOC|BODY)', line):
                        continue
                    #ignore case statement of switch
                    if re.match('^\s*(case\s(.*):|default:|break;|goto)', res.group(3)):
                        continue
                    # At least one identifier or number on the line (ie: ignore alone brackets)
                    if re.match('^.*[a-zA-Z0-9]', res.group(3)) is None:
                        continue

                    if current_function:
                        self.modules[module].functions[current_function].total_lines += 1

                    if not res.group(1) in ('#####', "-"):
                        if current_function:
                            self.modules[module].functions[current_function].covered_lines += 1


            if self.verbose > 0:
                print "computing coverage for %s : done" % module
        else:
            if self.verbose > 1:
                print "computing coverage for %s : FAILED" % module

    def coverall(self):
        for module, name, extension in list_modules():
            self.cover(module, name, extension)
        for module, name, extension in list_modules():
            self.compute_coverage(module, name, extension)
        self.all = True

    def covercurrent(self):
        for module, name, extension in list_modules():
            ftags = "./coverage/%s/%s%s.TAGS" % (module, name, extension)
            fgcov = "./coverage/%s/%s%s.gcov" % (module, name, extension)
            if os.path.isfile(fgcov) and os.path.isfile(ftags):
                self.compute_coverage(module, name, extension)


cover = Cover()
if len(sys.argv) < 2:
    cover.covercurrent()
elif sys.argv[1] == 'all':
    cover.coverall()
else:
    print "Computing coverage for one file"
    res = re.match(r'^((?:[/A-Za-z0-9_]+[/])*([/A-Za-z0-9_]+))', sys.argv[1])
    if res:
        module, name = res.group(1, 2)
        extension = '.hpp'
        if '/rio/' in module:
            extension = '.h'
        cover.cover(module, name, extension)
        for module, name, extension in [(module, name, extension)]:
            cover.compute_coverage(module, name, extension)
    else:
        print "Input does not match expected format"
        sys.exit(0)

print "Coverage Results:"
g_covered = 0
g_no_coverage = 0
g_low_coverage = 0
g_total_number = 0
for m in cover.modules:
    covered = 0
    no_coverage = 0
    low_coverage = 0
    total_number = 0
    for fnl in sorted(cover.modules[m].functions):
        fn = cover.modules[m].functions[fnl]
        total_number += 1
        if fn.covered_lines == 0:
            print "WARNING: NO COVERAGE %s%s:%s [%s] %s/%s" % (m, cover.modules[m].extension,
                                                fnl, fn.name, fn.covered_lines, fn.total_lines)
            no_coverage += 1
        elif fn.covered_lines * 100 < fn.total_lines * 50:
            print "WARNING: LOW COVERAGE %s%s:%s [%s] %s/%s" % (m, cover.modules[m].extension,
                                                fnl, fn.name, fn.covered_lines, fn.total_lines)
            low_coverage += 1
        else:
            print "COVERAGE %s%s:%s [%s] %s/%s" % (m, cover.modules[m].extension,
                                                fnl, fn.name, fn.covered_lines, fn.total_lines)
            covered += 1
    print "MODULE %s : %s COVERED, %s LOW_COVERAGE, %s NO_COVERAGE in %s TOTAL" % (m, covered,
                                                low_coverage, no_coverage, total_number)
    g_covered += covered
    g_no_coverage += no_coverage
    g_low_coverage += low_coverage
    g_total_number += total_number
    covered_rate = 0
    if g_total_number != 0:
        covered_rate = 100 * g_covered / g_total_number
if cover.all:
    print "ALL MODULES : %s COVERED (%s%%), %s LOW_COVERAGE, %s NO_COVERAGE in %s TOTAL" % (g_covered, covered_rate , g_low_coverage, g_no_coverage, g_total_number)
