from time import time
import os
import sys


WIKI = False
sorts=["heap", "merge", "quick"]
if (WIKI):
    sorts.append("wiki")
tests=["rand", "sort", "rev", "part"]
runs = 5
trials = 5
outdir = "stats"
datadir = '{}/data'.format(outdir)
progname = "mergesort_bench"
try:
    elts = sys.argv[1]
except:
    elts = 20

if (not os.path.isdir(datadir)):
    os.makedirs(datadir)

for test in tests:
    files = []
    for sort in sorts:
        filename = '{}/{}{}'.format(datadir, test, sort)
        files.append(filename)
        with open(filename, 'w+') as f:
            for _ in range(trials):
                start = time()
                ret = os.system('./{} {} {} {} {}'.format(progname, sort, test, runs, elts))
                total = time() - start
                if (ret):
                    sys.exit("Bench program failed. Did you remember to compile it?")
                f.write('{}\n'.format(str(total)))
            f.close()
    with open('{}/{}'.format(outdir, test), 'w+') as f:
        command = 'ministat -s -w 60 '
        for filename in files:
            command += '{} '.format(filename)
        command += '> {}/{}stats'.format(outdir, test)
        os.system(command)

