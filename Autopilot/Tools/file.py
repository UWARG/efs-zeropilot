#!/usr/bin/env python3

import sys
import datetime

dirn = './'
filename = sys.argv[1]
author = "Ian Frosst"

copy = '''\
/**
 * @file {0}
 * @author {1}
 * @date {2}
 * @copyright Waterloo Aerial Robotics Group {4} \\n
 *  https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 */\n
'''

fnc = filename + '.c'
fnh = filename + '.h'

with open(dirn + 'Src/' + fnc, 'w') as f:
    f.write(copy.format(fnc, author, datetime.date.today().isoformat(), datetime.date.today().year))
    f.write('#include "' + fnh + '"\n')


with open(dirn + 'Inc/' + fnh, 'w') as f:
    f.write(copy.format(fnh, author, datetime.date.today().isoformat(), datetime.date.today().year))
    defn = filename.upper() + '_H'
    f.write('#ifndef ' + defn + '\n')
    f.write('#define ' + defn + '\n' * 3)
    f.write('#endif /* ' + defn + ' */\n')
