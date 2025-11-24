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

#include "SMESH_Meshio.h"

// SALOME KERNEL includes
#include <SALOMEDS_Tool.hxx>
#include <Basics_DirUtils.hxx>
#include <SALOME_Exception.hh>
#include <utilities.h>

// Qt
#include <QFileInfo>

#include <fstream>


/*!
  Constructor
*/
SMESH_Meshio::SMESH_Meshio()
{
  CreateErrorFileName();
}

/*!
  Constructor
*/
SMESH_Meshio::SMESH_Meshio(const QString& selectedFilter)
: SMESH_Meshio()
{
  mySelectedFilter = selectedFilter;
}

/*!
  Destructor
*/
SMESH_Meshio::~SMESH_Meshio()
{
  RemoveTempFile();
  RemoveErrorFile();
}

/*!
  Convert file with meshio convert command
*/
void SMESH_Meshio::Convert(const QString& sourceFileName, const QString& targetFileName, bool isImport) const
{
  // Execute meshio convert command
  SMESHLibConverter::SMESHExternalConverter convertLib = SMESH_Meshio::GetLibraryForExtension(mySelectedFilter);
  QString commandExecutable;
  QString cmdConvertOpt = "";
  QString cmdOutputOpt = "";
  QString cmdSaveOpts = "";

  // Conversion QString -> enum
  // SMESHLibConverter::SMESHExternalConverter libNameEnum = SMESHLibConverter::fromQString(convertLib);
  switch (convertLib)
  {
    case SMESHLibConverter::SMESHExternalConverter::Gmsh:
      commandExecutable = "gmsh";
      cmdConvertOpt = "-format";
      cmdOutputOpt = "-o";
      cmdSaveOpts = "-save_all -save";
      break;

    case SMESHLibConverter::SMESHExternalConverter::MeshIo:
      commandExecutable = IsModernMeshioVersion() ? "meshio convert" : "meshio-convert";
      cmdConvertOpt = "-o";
      break;

    case SMESHLibConverter::SMESHExternalConverter::All:
    case SMESHLibConverter::SMESHExternalConverter::Unknown:
    default:
      MESSAGE("Unknown library for conversion");
      return;
  }


  // if (convertLib == SMESHLibConverter::Gmsh)
  // {
  //   commandExecutable = "gmsh";
  //   cmdConvertOpt = "-format";
  //   cmdOutputOpt = "-o";
  //   cmdSaveOpts = "-save_all -save";
  // }
  // else if (convertLib == SMESHLibConverter::MeshIo)
  // {
  //   commandExecutable = IsModernMeshioVersion() ? "meshio convert" : "meshio-convert";
  //   cmdConvertOpt = "-o";
  // }
  // else
  // {
  //   MESSAGE("Unknown library for conversion");
  //   return;
  // }
  
  // const QString convert = IsModernMeshioVersion() ? "meshio convert " : "meshio-convert ";
  QString optArgs = isImport ? "med" : GetConvertOptArgs(convertLib);


  // Build command parts
  QStringList cmdParts;
  cmdParts << commandExecutable;
  if (!cmdConvertOpt.isEmpty()) cmdParts << cmdConvertOpt << optArgs;
  else cmdParts << optArgs;
  cmdParts << sourceFileName;
  if (!cmdOutputOpt.isEmpty()) cmdParts << cmdOutputOpt << targetFileName;
  else cmdParts << targetFileName;
  if (!cmdSaveOpts.isEmpty()) cmdParts << cmdSaveOpts;
  cmdParts << "2>" << myErrorFileName;

  const std::string cmd = cmdParts.join(" ").toStdString();

  MESSAGE("Call system(\"" << cmd << "\") ...");

  const int status = system(cmd.c_str());
  MESSAGE("status: " << status);

  // Check results
  if (status != 0 || !Kernel_Utils::IsExists(targetFileName.toStdString()))
  {
    // Get all the output from an error file
    const std::string meshioErrors = ReadErrorsFromFile();
    MESSAGE("meshioErrors: \n" << meshioErrors);

    SALOME_CMOD::ExceptionStruct es;
    es.type = SALOME_CMOD::ExceptionType::BAD_PARAM;
    es.lineNumber = -1;
    es.text = CORBA::string_dup(
      ("MESHIO\nFailed system(\"" + cmd + "\").\n\n" + meshioErrors + "\nOperation canceled.").c_str());

    throw SALOME_CMOD::SALOME_Exception(es);
  }
}

/*!
  Returns a name for a file in default temp directory
  based on a target basename.
*/
QString SMESH_Meshio::CreateTempFileName(const QString& targetFileName)
{
  // Make a name for a temp file
  const QFileInfo fileInfo(targetFileName);
  const QString targetBaseName = fileInfo.baseName();
  const QString dirName(SALOMEDS_Tool::GetTmpDir().c_str());
  myTempFileName = dirName + targetBaseName + ".med";

  MESSAGE("targetFileName: " << targetFileName.toStdString());
  MESSAGE("myTempFileName: " << myTempFileName.toStdString());

  return myTempFileName;
}

/*!
  Removes the temporary file if we have any
*/
void SMESH_Meshio::RemoveTempFile()
{
  if (!myTempFileName.isEmpty())
  {
    std::remove(myTempFileName.toStdString().c_str());
    MESSAGE("Temp file " << myTempFileName.toStdString() << " was removed");
  }
}

/*!
  Returns meshio version string that has valid integer at least in the first position.
*/
QString SMESH_Meshio::GetMeshioVersion()
{
  auto IsVersionStringValid = [](const QString& version) -> bool
  {
    if (version.isEmpty())
    {
      return false;
    }

    // Check if we have an integer at least at the first position
    const QStringList curVersionNums = version.split('.');

    bool ok;
    const int firstNum = curVersionNums[0].toInt(&ok);
    if (!ok)
    {
      ERROR_MESSAGE("meshio version value is not valid!");
      return false;
    }

    MESSAGE("meshio version first number: " << firstNum);
    return true;
  };

  auto GetMeshioVersionFromEnv = []() -> QString
  {
    const char *envVar = std::getenv("MESHIO_VERSION");
    if (envVar && (envVar[0] != '\0'))
    {
      MESSAGE("MESHIO_VERSION: " << envVar);
      return envVar;
    }

    MESSAGE("MESHIO_VERSION is not set!");
    return {};
  };

  auto GetMeshioVersionHelper = [&]() -> QString
  {
    // Check if we can get a version from environment
    const QString meshioVersionEnv = GetMeshioVersionFromEnv();
    if (IsVersionStringValid(meshioVersionEnv))
    {
      return meshioVersionEnv;
    }

    // Try to guess a version by installed Python version
    const QString meshioVersionByPython = IsModernPythonVersion() ? "5" : "4";
    MESSAGE("meshio version was defined by Python version: " << meshioVersionByPython.toStdString());

    return meshioVersionByPython;
  };

  static const QString meshioVersion = GetMeshioVersionHelper();
  return meshioVersion;
};

/*!
  Returns true if we're going to use meshio version of 5.0 or greater.
*/
bool SMESH_Meshio::IsModernMeshioVersion()
{
  // It's a version when meshio commands were changed from using
  // many executables for each operation to one for everything (meshio 5.0).
  // For example, from
  // meshio-convert input.msh output.vtk
  // to
  // meshio convert input.msh output.vtk

  auto IsModernVersion = [&]() -> bool
  {
    const QString curVersion = GetMeshioVersion();

    const int minReqVersion = 5;
    const QStringList curVersionNums = curVersion.split('.');
    if (minReqVersion > curVersionNums[0].toInt())
    {
      return false;
    }

    return true;
  };

  static const bool isModern = IsModernVersion();
  return isModern;
}

/*!
  Returns true if meshio is installed
*/
bool SMESH_Meshio::IsMeshioInstalled()
{
  auto IsAllowedFromEnvironment = []() -> bool
  {
    const QString curVersion = GetMeshioVersion();

    // Check if we explicitly set off using of meshio from environment
    const QStringList curVersionNums = curVersion.split('.');
    const int firstNum = curVersionNums[0].toInt();
    if (firstNum <= 0)
    {
      MESSAGE("meshio was set as not installed from an environment");
      return false;
    }

    return true;
  };

  auto IsAbleToCallMeshio = []() -> bool
  {
    // Try to call meshio to check if it's present
    const std::string cmd =
      SMESH_Meshio::IsModernMeshioVersion() ? "meshio --version" : "meshio-info --version";

    const int status = system(cmd.c_str());
    MESSAGE("status: " << status);

    return status == 0;
  };

  static const bool isInstalled = IsAllowedFromEnvironment() && IsAbleToCallMeshio();
  return isInstalled;
}

/*!
  Returns true if current Python equal or newer than required version for
  meshio release from 5.0 and greater.
*/
bool SMESH_Meshio::IsModernPythonVersion()
{
  // It's a version when meshio commands were changed from using
  // many executables for each operation to one for everything (meshio 5.0).
  // For example, from
  // meshio-convert input.msh output.vtk
  // to
  // meshio convert input.msh output.vtk
 
  const std::vector<int> minReqVersionNums = { 3, 7 };

  auto GetCurrentVersion = []() -> QString
  {
    const char *envVar = std::getenv("PYTHON_VERSION");
    if (envVar && (envVar[0] != '\0'))
    {
      return envVar;
    }

    return {};
  };

  auto IsModernVersion = [&]() -> bool
  {
    const QString curVersion = GetCurrentVersion();
    MESSAGE("Current Python version: " << curVersion.toStdString())
    if (curVersion.length() < 3)
      return false;

    int index = 0;
    const QStringList curVersionNums = curVersion.split('.');
    for (const int minVersion : minReqVersionNums)
    {
      if (minVersion > curVersionNums[index].toInt())
      {
        return false;
      }

      ++index;
    }

    return true;
  };

  static const bool isModern = IsModernVersion();
  return isModern;
}

/*!
  Creates a temp file for error output from meshio command.
*/
void SMESH_Meshio::CreateErrorFileName()
{
  // Make a name for a temp file
  const QString fileName = "meshio_errors.txt";
  const QString dirName(SALOMEDS_Tool::GetTmpDir().c_str());
  myErrorFileName = dirName + fileName;
  
  MESSAGE("myErrorFileName: " << myErrorFileName.toStdString());
}

/*!
  Removes the temporary error output file if we have any
*/
void SMESH_Meshio::RemoveErrorFile()
{
  if (!myErrorFileName.isEmpty())
  {
    std::remove(myErrorFileName.toStdString().c_str());
    MESSAGE("Temp file " << myErrorFileName.toStdString() << " was removed");
  }
}

/*!
  Read all errors output from a temp file
*/
std::string SMESH_Meshio::ReadErrorsFromFile() const
{
  if (myErrorFileName.isEmpty())
    return {};

  std::ifstream fileStream(myErrorFileName.toStdString());
  if(fileStream)
  {
    std::ostringstream ss;
    ss << fileStream.rdbuf();

    return ss.str();
  }

  return {};
}

/*!
  Returns the label part of a filter string (before '(').
  Example: "VTK (*.vtk)" => "VTK"
*/
QString SMESH_Meshio::GetFilterLabel(QString filter) const
{
  const int index = filter.indexOf('(');
  if (index > 0)
    return filter.left(index).trimmed();

  return filter.trimmed();
}

/*!
  Get optional arguments for meshio convert command
*/
// QString SMESH_Meshio::GetConvertOptArgs(SMESHLibConverter::SMESHExternalConverter convertLib) const
// {
//   if (mySelectedFilter.isEmpty())
//     return mySelectedFilter;

//   const QString filterLabel = SMESH_Meshio::GetFilterLabel(mySelectedFilter);

//   // First, try to find a specific match for (convertLib, filterLabel)
//   auto it = options.find({ convertLib, filterLabel });
//   if (it != options.end())
//     return it->second;

//   // If not found, try a generic match for (All, filterLabel)
//   it = options.find({ SMESHLibConverter::SMESHExternalConverter::All, filterLabel });
//   if (it != options.end())
//     return it->second;

//   // No match found, return empty
//   return {};
// }

QString SMESH_Meshio::GetConvertOptArgs(SMESHLibConverter::SMESHExternalConverter convertLib) const
{
    if (mySelectedFilter.isEmpty())
        return {};

    // Convertir le filtre sélectionné en enum SMESHFormat
    SMESHLibConverter::SMESHFormat fmt = SMESHLibConverter::fromLabel(mySelectedFilter);
    if (fmt == SMESHLibConverter::SMESHFormat::Unknown)
        return {};

    // Chercher les infos du format
    auto itFmt = SMESHLibConverter::FormatTable.find(fmt);
    if (itFmt == SMESHLibConverter::FormatTable.end())
        return {};

    const auto& info = itFmt->second;

    // Cas spécifiques selon la librairie
    if (convertLib == SMESHLibConverter::SMESHExternalConverter::Gmsh) {
        // Retourner directement l’extension définie dans FormatTable
        return info.extension;
    }
    else if (convertLib == SMESHLibConverter::SMESHExternalConverter::MeshIo) {
        return info.extension;
    }
    else if (convertLib == SMESHLibConverter::SMESHExternalConverter::All) {
        return info.extension;
    }

    return {};
}



/*!
  find library for extension
*/
SMESHLibConverter::SMESHExternalConverter SMESH_Meshio::GetLibraryForExtension(const QString& selectedFilter)
{
    if (selectedFilter.isEmpty())
        return SMESHLibConverter::SMESHExternalConverter::Unknown;

    for (const auto& [fmt, info] : SMESHLibConverter::FormatTable)
    {
        // Comparaison sur toute la chaîne
        if (info.label.compare(selectedFilter, Qt::CaseInsensitive) == 0)
            return info.lib;
    }

    return SMESHLibConverter::SMESHExternalConverter::Unknown;
}

/*!
  Map of libraries and supported filters (labels)
*/
// const std::map<SMESHLibConverter::SMESHExternalConverter, std::vector<QString>> SMESH_Meshio::libExtensions = {
//     { SMESHLibConverter::SMESHExternalConverter::Gmsh, {
//         "Gmsh 1 (*.msh)",
//         "Gmsh 2 (*.msh)",
//         "Gmsh 2.2 (*.msh)",
//         "Gmsh 3 (*.msh)",
//         "Gmsh 4 (*.msh)",
//         "Gmsh 4.0 (*.msh)",
//         "Gmsh 4.1 (*.msh)",
//         "MAIL (*.mail)",
//         "Abaqus (*.inp)",
//         "CGNS (*.cgns)",
//         "MED/Salome (*.med)",
//         "Medit MESH (*.mesh)",
//         "Nastran (*.bdf)",
//         "OBJ (*.obj)",
//         "OFF (*.off)",
//         "PLY (*.ply)",
//         "STL (*.stl)",
//         "SU2 (*.su2)",
//         "Tecplot (*.dat)",
//         "VTK (*.vtk)",
//       }
//     },
//     { SMESHLibConverter::SMESHExternalConverter::MeshIo, {
//         // "Abaqus (*.inp)",
//         "ANSYS msh (*.msh)",
//         "AVS-UCD (*.avs)",
//         // "CGNS (*.cgns)",
//         "DOLFIN XML (*.xml)",
// #if !defined(WIN32)
//         "Exodus (*.e *.exo)",
// #endif
//         "FLAC3D (*.f3grid)",
//         "H5M (*.h5m)",
//         "Kratos/MDPA (*.mdpa)",
//         // "MED/Salome (*.med)",
//         "Medit MESHB (*.meshb)",
//         "Nastran (*.fem *.nas)",
//         "Netgen (*.vol *.vol.gz)",
//         // "OBJ (*.obj)",
//         // "OFF (*.off)",
//         "PERMAS (*.post *.post.gz *.dato *.dato.gz)",
//         // "PLY (*.ply)",
//         // "STL (*.stl)",
//         // "SU2 (*.su2)",
//         "SVG, 2D output only (*.svg)",
//         // "Tecplot (*.dat)",
//         "TetGen (*.node *.ele)",
//         "UGRID (*.ugrid)",
//         // "VTK (*.vtk)",
//         "VTU (*.vtu)",
//         "WKT, TIN (*.wkt)",
//         "XDMF (*.xdmf *.xmf)"
//       }
//     }
// };
