#!/usr/bin/env python3

import sys
import datetime
import getpass

dirn = './'
filename = sys.argv[1]
author = getpass.getuser()

copy = '''\
/**
 * @file {0}
 * @author {1}
 * @date {2}
 * @copyright Waterloo Aerial Robotics Group {3} \\n
 *  https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 */\n
'''

fnc = filename + '.c'
fnh = filename + '.h'

h_templ = '''
#ifndef {0}
#define {0}

#ifdef __cplusplus
extern "C" {{
#endif

/* declarations go here */

#ifdef __cplusplus
}}
#endif

#endif /* {0} */
'''.format(filename.upper() + '_H')

with open(dirn + 'Src/' + fnc, 'w') as f:
    f.write(copy.format(fnc, author, datetime.date.today().isoformat(), datetime.date.today().year))
    f.write('#include "' + fnh + '"\n')


with open(dirn + 'Inc/' + fnh, 'w') as f:
    f.write(copy.format(fnh, author, datetime.date.today().isoformat(), datetime.date.today().year))
    f.write(h_templ)
