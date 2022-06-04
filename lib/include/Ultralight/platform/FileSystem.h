///
/// @file FileSystem.h
///
/// @brief The header for the FileSystem interface.
///
/// @author
///
/// This file is a part of Ultralight, a next-generation HTML renderer.
///
/// Website: <http://ultralig.ht>
///
/// Copyright (C) 2022 Ultralight, Inc. All rights reserved.
///
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/String.h>
#include <Ultralight/Buffer.h>

namespace ultralight {

///
/// @brief  FileSystem interface.
///
/// This is used for loading File URLs (eg, <file:///page.html>).
///
/// You can provide the library with your own FileSystem implementation so that file assets are
/// loaded from your own pipeline (useful if you would like to encrypt/compress your file assets or
/// ship it in a custom format).
///
/// AppCore automatically provides a platform-specific implementation of this that loads files from
/// a local directory when you call App::Create().
///
/// If you are using Renderer::Create() instead, you will need to provide your own implementation
/// via `Platform::instance().set_file_system(). For convenience, you can still use AppCore's file
/// system implementation-- see the helper functions defined in <AppCore/Platform.h>.
///
/// To provide your own custom FileSystem implementation, you should inherit from this class,
/// handle the virtual member functions, and then pass an instance of your class to
/// `Platform::instance().set_file_system()` before calling Renderer::Create() or App::Create().
///
class UExport FileSystem {
 public:
  virtual ~FileSystem();

  ///
  /// Check if file path exists, return true if exists.
  ///
  virtual bool FileExists(const String& file_path) = 0;

  ///
  /// Get the mime-type of the file (eg "text/html").
  /// 
  /// This is usually determined by analyzing the file extension.
  /// 
  /// If a mime-type cannot be determined, this should return "application/unknown".
  ///
  virtual String GetFileMimeType(const String& file_path) = 0;

  ///
  /// Get the charset / encoding of the file (eg "utf-8", "iso-8859-1").
  /// 
  /// This is only applicable for text-based files (eg, "text/html", "text/plain") and is usually
  /// determined by analyzing the contents of the file.
  /// 
  /// If a charset cannot be determined, a safe default to return is "utf-8".
  /// 
  virtual String GetFileCharset(const String& file_path) = 0;

  ///
  /// Open file for reading and map it to a Buffer.
  /// 
  /// To minimize copies, you should map the requested file into memory and use Buffer::Create()
  /// to wrap the data pointer (unmapping should be performed in the destruction callback).
  /// 
  /// If the file was unable to be opened, you should return nullptr for this value.
  /// 
  virtual RefPtr<Buffer> OpenFile(const String& file_path) = 0;
};

} // namespace ultralight
