#!/usr/bin/env python3
#
# Copyright 2019 The Chromium Authors
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""Creates a GN include file for building dav1d from source."""

__author__ = "dalecurtis@chromium.org (Dale Curtis)"

import datetime
import glob
import os

_COPYRIGHT = """# Copyright %d The Chromium Authors
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# NOTE: this file is autogenerated by dav1d/generate_sources.py - DO NOT EDIT.

""" % (
    datetime.datetime.now().year)


def _Glob(pattern):
    # Replace path separator. Needed when running on Windows.
    return [i.replace(os.sep, '/') for i in glob.glob(pattern)]


def _WriteArray(fd, var_name, array, filter_list=[], last_entry=False):
    if len(array) == 0:
        fd.write("%s = []\n" % var_name)
        return

    fd.write("%s = [\n" % var_name)
    for item in sorted(array):
        if item not in filter_list:
            fd.write("  \"%s\",\n" % item)
    fd.write("]\n")
    if not last_entry:
        fd.write("\n")


def _WriteGn(fd):
    fd.write(_COPYRIGHT)

    # NOTE: $arch_template_sources are empty as of Dec 2022. If they remain
    # empty over the next few rolls we should remove them.
    _WriteArray(fd, "x86_asm_sources", _Glob("libdav1d/src/x86/*.asm"),
                ["libdav1d/src/x86/filmgrain_common.asm"])
    _WriteArray(fd, "x86_template_sources", _Glob("libdav1d/src/x86/*_tmpl.c"))

    _WriteArray(
        fd, "arm32_asm_sources", _Glob("libdav1d/src/arm/32/*.S"),
        _Glob("libdav1d/src/arm/32/*_tmpl.S") + ["libdav1d/src/arm/32/util.S"])
    _WriteArray(
        fd, "arm64_asm_sources", _Glob("libdav1d/src/arm/64/*.S"),
        _Glob("libdav1d/src/arm/64/*_tmpl.S") + ["libdav1d/src/arm/64/util.S"])
    _WriteArray(fd, "arm_template_sources", _Glob("libdav1d/src/arm/*_tmpl.c"))

    template_sources = _Glob("libdav1d/src/*_tmpl.c")
    _WriteArray(fd, "template_sources", template_sources)

    # Generate list of sources which need to be compiled multiple times with the
    # correct -DBIT_DEPTH=8|10 option specified each time.
    _WriteArray(fd, "c_headers", _Glob("libdav1d/src/*.h"))
    _WriteArray(fd, "c_sources", _Glob("libdav1d/src/*.c"), template_sources,
                last_entry=True)


def main():
    with open("dav1d_generated.gni", "w") as fd:
        _WriteGn(fd)


if __name__ == "__main__":
    main()