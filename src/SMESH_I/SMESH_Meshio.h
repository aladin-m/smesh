// Copyright (C) 2007-2025  CEA, EDF, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// SMESH SMESH_I : support of import / export with meshio library
// File   : SMESH_Meshio.h
// Author : Konstantin Leontev, Open CASCADE S.A.S.
//
#ifndef SMESH_MESHIO_H
#define SMESH_MESHIO_H

#include "SMESH.hxx"

#include <QString>
#include <QStringList>

#include <map>
#include <vector>
#include <cstring>

namespace SMESHIOConverter {

    // Enum representing supported external converters
    enum class ExternalConverter {
        Gmsh,
        MeshIo,
        Unknown
    };

    // Convert enum -> String
    inline std::string toString(ExternalConverter conv) noexcept {
        switch (conv) {
            case ExternalConverter::Gmsh:    return "gmsh";
            case ExternalConverter::MeshIo:  return "meshio";
        }
        return "unknown"; 
    }

    // Enum representing supported mesh formats
    enum class Extension {
        Gmsh1,
        Gmsh2,
        Gmsh22,
        Gmsh3,
        Gmsh4,
        Gmsh40,
        Gmsh41,
        Mail,
        Abaqus,
        Cgns,
        Med,
        MeditMesh,
        NastranBdf,
        Obj,
        Off,
        Ply,
        Stl,
        Su2,
        Tecplot,
        Vtk,
        Ansys,
        AvsUcd,
        DolfinXml,
        Exodus,
        Flac3d,
        H5m,
        Kratos,
        MeditMeshb,
        NastranFem,
        Netgen,
        Permas,
        Svg,
        Tetgen,
        Ugrid,
        Vtu,
        Wkt,
        Xdmf,
        Unknown,
    };

    // Structure storing metadata for each format
    struct ExtensionInfo {
        QString label;                 // label (e.g. "Gmsh 2.2 (*.msh)")
        QString extension;             // Primary extension or conversion option (e.g. "msh41")
        std::map<ExternalConverter, QString> converters; // Map of converter -> option string (e.g. {{Gmsh,"msh"}, {MeshIo,"msh"}})
    };

     /*!
        Mapping table between each mesh format (Extension) and its metadata.
        
        Each entry associates:
          - Extension (key)
          - ExtensionInfo containing:
              • label: string for Qt dialogs (e.g. "Gmsh 2.2 (*.msh)")
              • extension: primary extension or identifier for the format (e.g. "msh", "vtk")
              • converters: a map of external converters and their specific option strings
                (e.g. {{ExternalConverter::Gmsh, "msh22"}, {ExternalConverter::MeshIo, "gmsh22"}})
    */
    static const std::map<Extension, ExtensionInfo> ExtensionMap = {
        // Gmsh-specific formats
        { Extension::Gmsh40,    { "Gmsh 4.0 (*.msh)",             "msh",          { {ExternalConverter::Gmsh, "msh40"}, {ExternalConverter::MeshIo, "gmsh40"} } } },
        { Extension::Gmsh41,    { "Gmsh 4.1 (*.msh)",             "msh",          { {ExternalConverter::Gmsh, "msh41"}, {ExternalConverter::MeshIo, "gmsh"} } } },
        { Extension::Gmsh1,     { "Gmsh 1 (*.msh)",               "msh",          { {ExternalConverter::Gmsh, "msh1"}, {ExternalConverter::MeshIo, "gmsh1"} } } },
        { Extension::Gmsh2,     { "Gmsh 2 (*.msh)",               "msh",          { {ExternalConverter::Gmsh, "msh2"}, {ExternalConverter::MeshIo, "gmsh22"} } } },
        { Extension::Gmsh22,    { "Gmsh 2.2 (*.msh)",             "msh",          { {ExternalConverter::Gmsh, "msh22"} } } },
        { Extension::Gmsh3,     { "Gmsh 3 (*.msh)",               "msh",          { {ExternalConverter::Gmsh, "msh3"} } } },
        { Extension::Gmsh4,     { "Gmsh 4 (*.msh)",               "msh",          { {ExternalConverter::Gmsh, "msh4"} } } },

        { Extension::Mail,      { "MAIL (*.mail)",                "mail",         { {ExternalConverter::Gmsh, "mail"} } } },
        { Extension::Abaqus,    { "Abaqus (*.inp)",               "inp",          { {ExternalConverter::Gmsh, "inp"} } } },
        { Extension::Cgns,      { "CGNS (*.cgns)",                "cgns",         { {ExternalConverter::Gmsh, "cgns"} } } },
        { Extension::Med,       { "MED/Salome (*.med)",           "med",          { {ExternalConverter::Gmsh, "med"} } } },
        { Extension::MeditMesh, { "Medit MESH (*.mesh)",          "mesh",         { {ExternalConverter::Gmsh, "mesh"} } } },
        { Extension::NastranBdf,{ "Nastran (*.bdf)",              "bdf",          { {ExternalConverter::Gmsh, "bdf"} } } },
        { Extension::Obj,       { "OBJ (*.obj)",                  "obj",          { {ExternalConverter::Gmsh, "obj"} } } },
        { Extension::Off,       { "OFF (*.off)",                  "off",          { {ExternalConverter::Gmsh, "off"} } } },
        { Extension::Ply,       { "PLY (*.ply)",                  "ply",          { {ExternalConverter::Gmsh, "ply"} } } },
        { Extension::Stl,       { "STL (*.stl)",                  "stl",          { {ExternalConverter::Gmsh, "stl"} } } },
        { Extension::Su2,       { "SU2 (*.su2)",                  "su2",          { {ExternalConverter::Gmsh, "su2"} } } },
        { Extension::Tecplot,   { "Tecplot (*.dat)",              "dat",          { {ExternalConverter::Gmsh, "dat"} } } },
        { Extension::Vtk,       { "VTK (*.vtk)",                  "vtk",          { {ExternalConverter::Gmsh, "vtk"} } } },

        // MeshIo-specific formats
        { Extension::Ansys,     { "ANSYS msh (*.msh)",            "ansys",        { {ExternalConverter::MeshIo, "ansys"} } } },
        { Extension::AvsUcd,    { "AVS-UCD (*.avs)",              "avs",          { {ExternalConverter::MeshIo, "avsucd"} } } },
        { Extension::DolfinXml, { "DOLFIN XML (*.xml)",           "xml",          { {ExternalConverter::MeshIo, "dolfin-xml"} } } },
        { Extension::Exodus,    { "Exodus (*.e *.exo)",           "exo",          { {ExternalConverter::MeshIo, "exodus"} } } },
        { Extension::Flac3d,    { "FLAC3D (*.f3grid)",            "f3grid",       { {ExternalConverter::MeshIo, "flac3d"} } } },
        { Extension::H5m,       { "H5M (*.h5m)",                  "h5m",          { {ExternalConverter::MeshIo, "h5m"} } } },
        { Extension::Kratos,    { "Kratos/MDPA (*.mdpa)",         "mdpa",         { {ExternalConverter::MeshIo, "mdpa"} } } },
        { Extension::MeditMeshb,{ "Medit MESHB (*.meshb)",        "meshb",        { {ExternalConverter::MeshIo, "medit"} } } },
        { Extension::NastranFem,{ "Nastran (*.fem *.nas)",        "nas",          { {ExternalConverter::MeshIo, "nastran"} } } },
        { Extension::Netgen,    { "Netgen (*.vol *.vol.gz)",      "vol",          { {ExternalConverter::MeshIo, "netgen"} } } },
        { Extension::Permas,    { "PERMAS (*.post *.dato)",       "post",         { {ExternalConverter::MeshIo, "permas"} } } },
        { Extension::Svg,       { "SVG, 2D output only (*.svg)",  "svg",          { {ExternalConverter::MeshIo, "svg"} } } },
        { Extension::Tetgen,    { "TetGen (*.node *.ele)",        "node",         { {ExternalConverter::MeshIo, "tetgen"} } } },
        { Extension::Ugrid,     { "UGRID (*.ugrid)",              "ugrid",        { {ExternalConverter::MeshIo, "ugrid"} } } },
        { Extension::Vtu,       { "VTU (*.vtu)",                  "vtu",          { {ExternalConverter::MeshIo, "vtu"} } } },
        { Extension::Wkt,       { "WKT, TIN (*.wkt)",             "wkt",          { {ExternalConverter::MeshIo, "wkt"} } } },
        { Extension::Xdmf,      { "XDMF (*.xdmf *.xmf)",          "xdmf",         { {ExternalConverter::MeshIo, "xdmf"} } } }
    };

    // Convert enum -> label
    inline QString toLabel(Extension fmt) {
        auto it = ExtensionMap.find(fmt);
        return (it != ExtensionMap.end()) ? it->second.label : "Unknown";
    }

    // Convert enum -> extension
    inline QString toExtension(Extension fmt) {
        auto it = ExtensionMap.find(fmt);
        return (it != ExtensionMap.end()) ? it->second.extension : "";
    }

    // Get the converter-specific option string for a format
    inline QString toOption(Extension fmt, ExternalConverter conv) {
        auto it = ExtensionMap.find(fmt);
        if (it != ExtensionMap.end()) {
            auto convIt = it->second.converters.find(conv);
            if (convIt != it->second.converters.end())
                return convIt->second;
        }
        return "Unknown"; // fallback if converter not found
    }

    // Convert label -> enum
    inline Extension fromLabel(const QString& label) {
        for (const auto& [fmt, info] : ExtensionMap) {
            if (info.label.compare(label, Qt::CaseInsensitive) == 0)
                return fmt;
        }
        return Extension::Unknown;
    }

    // Convert extension -> enum
    inline Extension fromExtension(const QString& ext) {
        for (const auto& [fmt, info] : ExtensionMap) {
            if (info.extension.compare(ext, Qt::CaseInsensitive) == 0)
                return fmt;
        }
        return Extension::Unknown;
    }

    // Convert option string -> enum (converter-specific)
    inline Extension fromOption(const QString& opt, ExternalConverter conv) {
        for (const auto& [fmt, info] : ExtensionMap) {
            auto convIt = info.converters.find(conv);
            if (convIt != info.converters.end() &&
                convIt->second.compare(opt, Qt::CaseInsensitive) == 0)
                return fmt;
        }
        return Extension::Unknown;
    }

} // namespace SMESHIOConverter

class SMESH_I_EXPORT SMESH_Meshio
{
public:
  SMESH_Meshio();
  SMESH_Meshio(const QString& selectedFilter);
  virtual ~SMESH_Meshio();

  QString CreateTempFileName(const QString& targetFileName);
  void Convert(const QString& sourceFileName, const QString& targetFileName, bool isImport = false) const;

  static QString GetMeshioVersion();
  static bool IsModernMeshioVersion();
  static bool IsMeshioInstalled();
  static bool IsModernPythonVersion();

  static QString GetConverterVersion(SMESHIOConverter::ExternalConverter converter);
  static bool IsConverterInstalled(SMESHIOConverter::ExternalConverter converter = SMESHIOConverter::ExternalConverter::Gmsh);
  static SMESHIOConverter::ExternalConverter GetConverterForExtension(const QString& selectedFilter,  const QString& sourceFileName = QString());
  QString GetFilterLabel(QString filter) const;
  

private:
  void RemoveTempFile();
  void CreateErrorFileName();
  void RemoveErrorFile();
  std::string ReadErrorsFromFile() const;

  QString GetConvertOptArgs(SMESHIOConverter::ExternalConverter convertLib, const QString& sourceFileName = QString()) const;
  

private:
  QString myTempFileName;
  QString myErrorFileName;
  QString mySelectedFilter;
};

#endif // SMESH_MESHIO_H
