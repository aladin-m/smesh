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

    // // Convert enum -> const char*
    // inline const char* toCString(SMESHExternalConverter conv) noexcept {
    //     switch (conv) {
    //         case SMESHExternalConverter::Gmsh:   return "gmsh";
    //         case SMESHExternalConverter::MeshIo: return "meshio";
    //         case SMESHExternalConverter::All:    return "all";
    //         case SMESHExternalConverter::Unknown:    return "unknown";
    //     }
    //     return "unknown";
    // }

    // // Convert enum -> QString
    // inline QString toQString(SMESHExternalConverter conv) noexcept {
    //     return QString::fromUtf8(toCString(conv));
    // }

    // // Convert const char* -> enum
    // inline SMESHExternalConverter fromCString(const char* name) noexcept {
    //     if (!name) return SMESHExternalConverter::All;
    //     if (std::strcmp(name, "gmsh") == 0)   return SMESHExternalConverter::Gmsh;
    //     if (std::strcmp(name, "meshio") == 0) return SMESHExternalConverter::MeshIo;
    //     if (std::strcmp(name, "all") == 0)    return SMESHExternalConverter::All;
    //     if (std::strcmp(name, "unknown") == 0)    return SMESHExternalConverter::Unknown;
    //     return SMESHExternalConverter::All; // default fallback
    // }

    // // Convert QString -> enum
    // inline SMESHExternalConverter fromQString(const QString& name) noexcept {
    //     QString lower = name.toLower();
    //     if (lower == "gmsh")   return SMESHExternalConverter::Gmsh;
    //     if (lower == "meshio") return SMESHExternalConverter::MeshIo;
    //     if (lower == "all")    return SMESHExternalConverter::All;
    //     if (lower == "unknown")    return SMESHExternalConverter::Unknown;
    //     return SMESHExternalConverter::All;
    // }

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
        SMESHExternalConverter lib;    // Associated library (e.g. Gmsh, MeshIo, ...)
    };

    /*!
        Mapping table between each mesh format (SMESHFormat) and its metadata.
       
        Each entry associates:
          - SMESHFormat (key)
          - FormatInfo containing:
              • label: human-readable string for Qt dialogs (e.g. "Gmsh 2.2 ( *.msh)")
              • extension: extension or identifier used for conversion options (e.g. "msh22")
              • lib: external library responsible for the format (SMESHExternalConverter::Gmsh, MeshIo, etc.)
     */
    static const std::map<SMESHFormat, FormatInfo> FormatTable = {
        // Gmsh-specific formats
        { SMESHFormat::Gmsh1,     { "Gmsh 1 (*.msh)", "msh1", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::Gmsh2,     { "Gmsh 2 (*.msh)", "msh2", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::Gmsh22,    { "Gmsh 2.2 (*.msh)", "msh22", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::Gmsh3,     { "Gmsh 3 (*.msh)", "msh3", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::Gmsh4,     { "Gmsh 4 (*.msh)", "msh4", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::Gmsh40,    { "Gmsh 4.0 (*.msh)", "msh40", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::Gmsh41,    { "Gmsh 4.1 (*.msh)", "msh41", SMESHExternalConverter::Gmsh } },

        { SMESHFormat::Mail,      { "MAIL (*.mail)", "mail", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::Abaqus,    { "Abaqus (*.inp)", "inp", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::Cgns,      { "CGNS (*.cgns)", "cgns", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::Med,       { "MED/Salome (*.med)", "med", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::MeditMesh, { "Medit MESH (*.mesh)", "mesh", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::NastranBdf,{ "Nastran (*.bdf)", "bdf", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::Obj,       { "OBJ (*.obj)", "obj", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::Off,       { "OFF (*.off)", "off", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::Ply,       { "PLY (*.ply)", "ply", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::Stl,       { "STL (*.stl)", "stl", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::Su2,       { "SU2 (*.su2)", "su2", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::Tecplot,   { "Tecplot (*.dat)", "dat", SMESHExternalConverter::Gmsh } },
        { SMESHFormat::Vtk,       { "VTK (*.vtk)", "vtk", SMESHExternalConverter::Gmsh } },

        // MeshIo-specific formats
        { SMESHFormat::Ansys,     { "ANSYS msh (*.msh)", "ansys", SMESHExternalConverter::MeshIo } },
        { SMESHFormat::AvsUcd,    { "AVS-UCD (*.avs)", "avs", SMESHExternalConverter::MeshIo } },
        { SMESHFormat::DolfinXml, { "DOLFIN XML (*.xml)", "xml", SMESHExternalConverter::MeshIo } },
        { SMESHFormat::Exodus,    { "Exodus (*.e *.exo)", "exo", SMESHExternalConverter::MeshIo } },
        { SMESHFormat::Flac3d,    { "FLAC3D (*.f3grid)", "f3grid", SMESHExternalConverter::MeshIo } },
        { SMESHFormat::H5m,       { "H5M (*.h5m)", "h5m", SMESHExternalConverter::MeshIo } },
        { SMESHFormat::Kratos,    { "Kratos/MDPA (*.mdpa)", "mdpa", SMESHExternalConverter::MeshIo } },
        { SMESHFormat::MeditMeshb,{ "Medit MESHB (*.meshb)", "meshb", SMESHExternalConverter::MeshIo } },
        { SMESHFormat::NastranFem,{ "Nastran (*.fem *.nas)", "nas", SMESHExternalConverter::MeshIo } },
        { SMESHFormat::Netgen,    { "Netgen (*.vol *.vol.gz)", "vol", SMESHExternalConverter::MeshIo } },
        { SMESHFormat::Permas,    { "PERMAS (*.post *.dato)", "post", SMESHExternalConverter::MeshIo } },
        { SMESHFormat::Svg,       { "SVG, 2D output only (*.svg)", "svg", SMESHExternalConverter::MeshIo } },
        { SMESHFormat::Tetgen,    { "TetGen (*.node *.ele)", "node", SMESHExternalConverter::MeshIo } },
        { SMESHFormat::Ugrid,     { "UGRID (*.ugrid)", "ugrid", SMESHExternalConverter::MeshIo } },
        { SMESHFormat::Vtu,       { "VTU (*.vtu)", "vtu", SMESHExternalConverter::MeshIo } },
        { SMESHFormat::Wkt,       { "WKT, TIN (*.wkt)", "wkt", SMESHExternalConverter::MeshIo } },
        { SMESHFormat::Xdmf,      { "XDMF (*.xdmf *.xmf)", "xdmf", SMESHExternalConverter::MeshIo } }
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
  // static const std::map<SMESHLibConverter::SMESHExternalConverter, std::vector<QString>> libExtensions;
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
