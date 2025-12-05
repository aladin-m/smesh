#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2025  CEA, EDF, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# =======================================
# Testing export/import of some simple shapes in different formats with gmsh library
#  File   : SMESH_GmshIOShapes.py
#  Module : SMESH

import tempfile
from pathlib import Path

from salome.kernel import salome
salome.salome_init()

from salome.kernel import GEOM
from salome.geom import geomBuilder

from salome.kernel import SMESH
from salome.smesh import smeshBuilder

from salome.kernel import SALOME_CMOD

import platform

# Constants
EXPORT_TITLE = 'Export'
IMPORT_TITLE = 'Import'
UNKNOWN_EXCEPTION = 'Unknown exception'
OUTPUT_DIVIDER = '\n==============================================='


geompy = geomBuilder.New()
smesh = smeshBuilder.New()


def make_plane():
    """
    Makes default plane for testing.
    """

    plane = geompy.MakeFaceHW(100, 100, 1)
    geompy.addToStudy( plane, 'plane' )

    return plane


def make_box():
    """
    Makes default box for testing.
    """

    box = geompy.MakeBoxDXDYDZ(200, 200, 200)
    geompy.addToStudy(box, 'box')

    return box


def make_mesh(shape, name):
    """
    Makes a mesh from a given shape.
    """
    res_mesh = smesh.Mesh(shape, name)
    NETGEN_1D_2D = res_mesh.Triangle(algo=smeshBuilder.NETGEN_1D2D)
    isDone = res_mesh.Compute()
    assert isDone

    smesh.SetName(res_mesh, name)
    smesh.SetName(NETGEN_1D_2D.GetAlgorithm(), 'NETGEN 1D-2D')
    smesh.SetName(res_mesh.GetMesh(), name)

    return res_mesh


def file_infos():
    """
    Returns all files extensions with the filters supported by gmsh.
    """

    infos = [
        ['.msh', 'Gmsh 4.0 (*.msh)'],
        ['.msh', 'Gmsh 4.1 (*.msh)'],
    ]
    return infos

def exception_handle(file_name, errors, operation_type, ex_text):
    """
    Pepares and saves an exception message.
    """

    ext = Path(file_name).suffix.upper()
    msg = '{} {} failed! File name: {}'.format(operation_type, ext, file_name)
    if ex_text:
        msg += ' Exception: ' + ex_text + OUTPUT_DIVIDER + '\n'

    errors.append(msg)


def export_mesh(mesh, file_name, filter='', errors=None):
    """
    Performs an export operation.
    """

    try:
        print('Export to file: ', file_name)

        mesh.ExportMESHIO(file_name, filter, mesh)
        return True

    except SALOME_CMOD.SALOME_Exception as ex:
        exception_handle(file_name, errors, EXPORT_TITLE, ex.details.text)

    except:
        exception_handle(file_name, errors, EXPORT_TITLE, UNKNOWN_EXCEPTION)

    return False


def import_file(file_name, filter='', errors=None):
    """
    Performs an import operation.
    """

    try:
        print('Import from file: ', file_name)

        ([mesh], status) = smesh.CreateMeshesFromMESHIO(file_name, filter)
        return mesh

    except SALOME_CMOD.SALOME_Exception as ex:
        exception_handle(file_name, errors, IMPORT_TITLE, ex.details.text)

    except:
        exception_handle(file_name, errors, IMPORT_TITLE, UNKNOWN_EXCEPTION)

    return None


def print_errors(errors):
    """
    Checks if we have got any saved error messages and print them.
    The test failed in this case.
    """

    if not len(errors):
        return

    print('\nErrors:\n')
    for idx, err in enumerate(errors):
        print('{:02}: {}'.format(idx, err))

    print(OUTPUT_DIVIDER)


def perform(mesh, name, errors):
    """
    Exports a given mesh and imports it back for each file format.
    """

    for info in file_infos():
        with tempfile.NamedTemporaryFile(suffix=info[0], prefix=name) as file:
            file.close() # prevents PermissionError on Windows
            if (export_mesh(mesh, file.name, info[1], errors)):
                file.close() # prevents PermissionError on Windows
                import_file(file.name, info[1], errors)


def test_box(errors):
    """
    Tests export / import of a mesh for a default box geometry.
    """

    box = make_box()

    mesh_name = 'Test_box_'
    mesh = make_mesh(box, mesh_name)
    perform(mesh, mesh_name, errors)


def test_plane(errors):
    """
    Tests export / import of a mesh for a default plane geometry.
    """

    plane = make_plane()

    mesh_name = 'Test_plane_'
    mesh = make_mesh(plane, mesh_name)
    perform(mesh, mesh_name, errors)


def test():
    """
    Creates meshes from plane and box and tries to export them to a temp files
    and import them back with different formats supported by gmsh library.
    Prints errors if we have any.
    """

    errors = []

    test_plane(errors)
    test_box(errors)

    print_errors(errors)
    assert not len(errors)


if __name__ == "__main__":
    test()
