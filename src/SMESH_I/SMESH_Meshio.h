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

namespace SMESHLibConverter {

    // Enum representing supported external converters
    enum class SMESHExternalConverter {
        Gmsh,
        MeshIo,
        All,
        Unknown
    };

    // Convert enum -> String
    inline std::string toString(SMESHExternalConverter conv) noexcept {
        switch (conv) {
            case SMESHExternalConverter::Gmsh:    return "gmshh";
            case SMESHExternalConverter::MeshIo:  return "meshio";
            case SMESHExternalConverter::All:     return "all";
            case SMESHExternalConverter::Unknown: return "unknown";
        }
        return "unknown"; 
    }

    // Enum representing supported mesh formats
    enum class SMESHFormat {
        Unknown,
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
        Xdmf
    };

    // Structure storing metadata for each format
    struct FormatInfo {
        QString label;                 // Human-readable label (e.g. "Gmsh 2.2 (*.msh)")
        QString extension;             // Primary extension or conversion option (e.g. "msh41")
        std::map<SMESHExternalConverter, QString> converters; // Map of converter -> option string (e.g. {{Gmsh,"msh"}, {MeshIo,"msh"}})
    };

     /*!
        Mapping table between each mesh format (SMESHFormat) and its metadata.
        
        Each entry associates:
          - SMESHFormat (key)
          - FormatInfo containing:
              • label: human-readable string for Qt dialogs (e.g. "Gmsh 2.2 (*.msh)")
              • extension: primary extension or identifier for the format (e.g. "msh", "vtk")
              • converters: a map of external converters and their specific option strings
                (e.g. {{SMESHExternalConverter::Gmsh, "msh22"}, {SMESHExternalConverter::MeshIo, "gmsh22"}})
    */
    static const std::map<SMESHFormat, FormatInfo> FormatTable = {
        // Gmsh-specific formats
        { SMESHFormat::Gmsh1,     { "Gmsh 1 (*.msh)",               "msh",          { {SMESHExternalConverter::Gmsh, "msh1"}, {SMESHExternalConverter::MeshIo, "gmsh1"} } } },
        { SMESHFormat::Gmsh2,     { "Gmsh 2 (*.msh)",               "msh",          { {SMESHExternalConverter::Gmsh, "msh2"}, {SMESHExternalConverter::MeshIo, "gmsh22"} } } },
        { SMESHFormat::Gmsh22,    { "Gmsh 2.2 (*.msh)",             "msh",          { {SMESHExternalConverter::Gmsh, "msh22"} } } },
        { SMESHFormat::Gmsh3,     { "Gmsh 3 (*.msh)",               "msh",          { {SMESHExternalConverter::Gmsh, "msh3"} } } },
        { SMESHFormat::Gmsh4,     { "Gmsh 4 (*.msh)",               "msh",          { {SMESHExternalConverter::Gmsh, "msh4"} } } },
        { SMESHFormat::Gmsh40,    { "Gmsh 4.0 (*.msh)",             "msh",          { {SMESHExternalConverter::Gmsh, "msh40"}, {SMESHExternalConverter::MeshIo, "gmsh40"} } } },
        { SMESHFormat::Gmsh41,    { "Gmsh 4.1 (*.msh)",             "msh",          { {SMESHExternalConverter::Gmsh, "msh41"}, {SMESHExternalConverter::MeshIo, "gmsh"} } } },

        { SMESHFormat::Mail,      { "MAIL (*.mail)",                "mail",         { {SMESHExternalConverter::Gmsh, "mail"} } } },
        { SMESHFormat::Abaqus,    { "Abaqus (*.inp)",               "inp",          { {SMESHExternalConverter::Gmsh, "inp"} } } },
        { SMESHFormat::Cgns,      { "CGNS (*.cgns)",                "cgns",         { {SMESHExternalConverter::Gmsh, "cgns"} } } },
        { SMESHFormat::Med,       { "MED/Salome (*.med)",           "med",          { {SMESHExternalConverter::Gmsh, "med"} } } },
        { SMESHFormat::MeditMesh, { "Medit MESH (*.mesh)",          "mesh",         { {SMESHExternalConverter::Gmsh, "mesh"} } } },
        { SMESHFormat::NastranBdf,{ "Nastran (*.bdf)",              "bdf",          { {SMESHExternalConverter::Gmsh, "bdf"} } } },
        { SMESHFormat::Obj,       { "OBJ (*.obj)",                  "obj",          { {SMESHExternalConverter::Gmsh, "obj"} } } },
        { SMESHFormat::Off,       { "OFF (*.off)",                  "off",          { {SMESHExternalConverter::Gmsh, "off"} } } },
        { SMESHFormat::Ply,       { "PLY (*.ply)",                  "ply",          { {SMESHExternalConverter::Gmsh, "ply"} } } },
        { SMESHFormat::Stl,       { "STL (*.stl)",                  "stl",          { {SMESHExternalConverter::Gmsh, "stl"} } } },
        { SMESHFormat::Su2,       { "SU2 (*.su2)",                  "su2",          { {SMESHExternalConverter::Gmsh, "su2"} } } },
        { SMESHFormat::Tecplot,   { "Tecplot (*.dat)",              "dat",          { {SMESHExternalConverter::Gmsh, "dat"} } } },
        { SMESHFormat::Vtk,       { "VTK (*.vtk)",                  "vtk",          { {SMESHExternalConverter::Gmsh, "vtk"} } } },

        // MeshIo-specific formats
        { SMESHFormat::Ansys,     { "ANSYS msh (*.msh)",            "ansys",        { {SMESHExternalConverter::MeshIo, "ansys"} } } },
        { SMESHFormat::AvsUcd,    { "AVS-UCD (*.avs)",              "avs",          { {SMESHExternalConverter::MeshIo, "avs"} } } },
        { SMESHFormat::DolfinXml, { "DOLFIN XML (*.xml)",           "xml",          { {SMESHExternalConverter::MeshIo, "xml"} } } },
        { SMESHFormat::Exodus,    { "Exodus (*.e *.exo)",           "exo",          { {SMESHExternalConverter::MeshIo, "exo"} } } },
        { SMESHFormat::Flac3d,    { "FLAC3D (*.f3grid)",            "f3grid",       { {SMESHExternalConverter::MeshIo, "f3grid"} } } },
        { SMESHFormat::H5m,       { "H5M (*.h5m)",                  "h5m",          { {SMESHExternalConverter::MeshIo, "h5m"} } } },
        { SMESHFormat::Kratos,    { "Kratos/MDPA (*.mdpa)",         "mdpa",         { {SMESHExternalConverter::MeshIo, "mdpa"} } } },
        { SMESHFormat::MeditMeshb,{ "Medit MESHB (*.meshb)",        "meshb",        { {SMESHExternalConverter::MeshIo, "meshb"} } } },
        { SMESHFormat::NastranFem,{ "Nastran (*.fem *.nas)",        "nas",          { {SMESHExternalConverter::MeshIo, "nas"} } } },
        { SMESHFormat::Netgen,    { "Netgen (*.vol *.vol.gz)",      "vol",          { {SMESHExternalConverter::MeshIo, "vol"} } } },
        { SMESHFormat::Permas,    { "PERMAS (*.post *.dato)",       "post",         { {SMESHExternalConverter::MeshIo, "post"} } } },
        { SMESHFormat::Svg,       { "SVG, 2D output only (*.svg)",  "svg",          { {SMESHExternalConverter::MeshIo, "svg"} } } },
        { SMESHFormat::Tetgen,    { "TetGen (*.node *.ele)",        "node",         { {SMESHExternalConverter::MeshIo, "node"} } } },
        { SMESHFormat::Ugrid,     { "UGRID (*.ugrid)",              "ugrid",        { {SMESHExternalConverter::MeshIo, "ugrid"} } } },
        { SMESHFormat::Vtu,       { "VTU (*.vtu)",                  "vtu",          { {SMESHExternalConverter::MeshIo, "vtu"} } } },
        { SMESHFormat::Wkt,       { "WKT, TIN (*.wkt)",             "wkt",          { {SMESHExternalConverter::MeshIo, "wkt"} } } },
        { SMESHFormat::Xdmf,      { "XDMF (*.xdmf *.xmf)",          "xdmf",         { {SMESHExternalConverter::MeshIo, "xdmf"} } } }
    };

    // Convert enum -> label
    inline QString toLabel(SMESHFormat fmt) {
        auto it = FormatTable.find(fmt);
        return (it != FormatTable.end()) ? it->second.label : "Unknown";
    }

    // Convert enum -> extension
    inline QString toExtension(SMESHFormat fmt) {
        auto it = FormatTable.find(fmt);
        return (it != FormatTable.end()) ? it->second.extension : "";
    }

    // Get the converter-specific option string for a format
    inline QString toOption(SMESHFormat fmt, SMESHExternalConverter conv) {
        auto it = FormatTable.find(fmt);
        if (it != FormatTable.end()) {
            auto convIt = it->second.converters.find(conv);
            if (convIt != it->second.converters.end())
                return convIt->second;
        }
        return "Unknown"; // fallback if converter not found
    }

    // Convert label -> enum
    inline SMESHFormat fromLabel(const QString& label) {
        for (const auto& [fmt, info] : FormatTable) {
            if (info.label.compare(label, Qt::CaseInsensitive) == 0)
                return fmt;
        }
        return SMESHFormat::Unknown;
    }

    // Convert extension -> enum
    inline SMESHFormat fromExtension(const QString& ext) {
        for (const auto& [fmt, info] : FormatTable) {
            if (info.extension.compare(ext, Qt::CaseInsensitive) == 0)
                return fmt;
        }
        return SMESHFormat::Unknown;
    }

    // Convert option string -> enum (converter-specific)
    inline SMESHFormat fromOption(const QString& opt, SMESHExternalConverter conv) {
        for (const auto& [fmt, info] : FormatTable) {
            auto convIt = info.converters.find(conv);
            if (convIt != info.converters.end() &&
                convIt->second.compare(opt, Qt::CaseInsensitive) == 0)
                return fmt;
        }
        return SMESHFormat::Unknown;
    }

} // namespace SMESHLibConverter

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
  static bool IsConvertLibInstalled(SMESHLibConverter::SMESHExternalConverter lib = SMESHLibConverter::SMESHExternalConverter::Gmsh);
  static SMESHLibConverter::SMESHExternalConverter GetLibraryForExtension(const QString& selectedFilter);
  QString GetFilterLabel(QString filter) const;
  

private:
  void RemoveTempFile();
  void CreateErrorFileName();
  void RemoveErrorFile();
  std::string ReadErrorsFromFile() const;

  QString GetConvertOptArgs(SMESHLibConverter::SMESHExternalConverter convertLib) const;
  

private:
  QString myTempFileName;
  QString myErrorFileName;
  QString mySelectedFilter;
};

#endif // SMESH_MESHIO_H
