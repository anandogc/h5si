# H5SI
# 
# Copyright (C) 2008, 2009  Anando Gopal Chatterjee
#
# Anando Gopal Chatterjee
# Indian Institute of Technology, Kanpur-208016
# UP, India
#
# anandogc@iitk.ac.in
#
# This file is part of H5SI.
#
# H5SI is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# H5SI is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with H5SI; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, U
#

# \file  CMakeLists.txt
# @author  A. G. Chatterjee

ADD_EXECUTABLE(read_write read_write.cc)
TARGET_LINK_LIBRARIES(read_write h5si ${SYSTEM_LIBRARIES})

IF (ENABLE_PARALLEL)
	ADD_EXECUTABLE(read_write_parallel read_write_parallel.cc)
	TARGET_LINK_LIBRARIES(read_write_parallel h5si ${SYSTEM_LIBRARIES})
ENDIF()

