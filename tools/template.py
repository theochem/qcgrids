#!/usr/bin/env python
# -*- coding: utf-8 -*-
# QCGrids is a numerical integration library for quantum chemistry.
# Copyright (C) 2011-2017 The QCGrids developers
#
# This file is part of QCGrids.
#
# QCGrids is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# QCGrids is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>
# --

from __future__ import print_function

import os
import sys
from stat import S_IREAD, S_IRGRP, S_IROTH, S_IWUSR
import urllib.request


def set_ro(fn):
    os.chmod(fn, S_IREAD | S_IRGRP | S_IROTH)


def unset_ro(fn):
    os.chmod(fn, S_IWUSR | S_IREAD)


def main():
    unique = []
    url = None
    with open(sys.argv[1]) as fh:
        for line in fh:
            if line.startswith('# EVERYTHING BELOW THIS LINE IS AUTOGENERATED.') or \
                    line.startswith('# EVERYTHING BELOW THIS LINE WILL BE COPIED INTO OTHER YMLs'):
                unique.append(line)
                break
            elif line.startswith('# TEMPLATED FROM '):
                url = line.split()[-1]
            unique.append(line)

    if url:
        with urllib.request.urlopen(url) as response:
            template = response.read().decode()

        template = template.split("\n")
        for i, l in enumerate(template):
            if l.startswith('# EVERYTHING BELOW THIS LINE WILL BE COPIED INTO OTHER YMLs'):
                template = template[i + 1:]
                break
        template = "\n".join(template)

        unset_ro(sys.argv[1])
        with open(sys.argv[1], "w") as fh:
            fh.write("".join(unique))
            fh.write(template)
        set_ro(sys.argv[1])


if __name__ == '__main__':
    main()
