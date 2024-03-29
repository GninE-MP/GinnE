//  tinygettext - A gettext replacement that works directly on .po files
//  Copyright (C) 2006 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "dictionary_manager.hpp"

#include <memory>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <algorithm>

#include "log_stream.hpp"
#include "po_parser.hpp"
#include "unix_file_system.hpp"

namespace tinygettext {

static bool has_suffix(const std::string& lhs, const std::string rhs)
{
  if (lhs.length() < rhs.length())
    return false;
  else
    return lhs.compare(lhs.length() - rhs.length(), rhs.length(), rhs) == 0;
}

DictionaryManager::DictionaryManager(const std::string& charset_) :
  dictionaries(),
  search_path(),
  charset(charset_),
  use_fuzzy(true),
  current_language(),
  current_dict(0),
  empty_dict(),
  filesystem(new UnixFileSystem)
{
}

DictionaryManager::~DictionaryManager()
{
  for(Dictionaries::iterator i = dictionaries.begin(); i != dictionaries.end(); ++i)
  {
    delete i->second;
  }
}

void
DictionaryManager::clear_cache()
{
  for(Dictionaries::iterator i = dictionaries.begin(); i != dictionaries.end(); ++i)
  {
    delete i->second;
  }
  dictionaries.clear();

  current_dict = 0;
}

Dictionary&
DictionaryManager::get_dictionary()
{
  if (current_dict)
  {
    return *current_dict; 
  }
  else
  {
    if (current_language)
    {
      current_dict = &get_dictionary(current_language);
      return *current_dict;
    }
    else
    {
      return empty_dict;
    }
  }
}

Dictionary&
DictionaryManager::get_dictionary(const Language& language, const std::string& textdomain )
{
  //log_debug << "Dictionary for language \"" << spec << "\" requested" << std::endl;
  //log_debug << "...normalized as \"" << lang << "\"" << std::endl;
  assert(language);

  Dictionaries::iterator i = dictionaries.find(language); 
  if (i != dictionaries.end())
  {
    return *i->second;
  }
  else // Dictionary for languages lang isn't loaded, so we load it
  {
    //log_debug << "get_dictionary: " << lang << std::endl;
    Dictionary* dict = &empty_dict;

    for (SearchPath::reverse_iterator p = search_path.rbegin(); p != search_path.rend(); ++p)
    {
      std::string best_filename;
      int best_score = 0;

      // If we don't have a textdomain, search for any .po files in this directory
      std::vector<std::string> files = std::vector<std::string>();
      std::vector<std::string> dirs = std::vector<std::string>();
      filesystem->open_directory(*p,files,dirs);
      for(std::vector<std::string>::iterator filename = files.begin(); filename != files.end(); filename++)
      {
        // check if filename matches requested language
        if (has_suffix(*filename, ".po") || has_suffix(*filename, ".pot"))
        { // ignore anything that isn't a .po file
          Language po_language = Language::from_env(filename->substr(0, filename->size()-3));
          if (!po_language)
          {
            log_warning << *filename << ": warning: ignoring, unknown language" << std::endl;
          }
          else
          {
            int score = Language::match(language, po_language);
                          
            if (score > best_score)
            {
              best_score = score;
              best_filename = *filename;
            }
          }
        }
      }

      // If we were given a text domain, we also search directories looking for ll_CC/textdomain.po
      if ( !textdomain.empty() )
      {
        for(std::vector<std::string>::iterator directory = dirs.begin(); directory != dirs.end(); directory++)
        {
          Language po_language = Language::from_env(*directory);
          std::string filename = *directory + "/" + textdomain + ".po";

          if (!filesystem->file_exists(*p + "/" + filename))
          {
            filename += "t";
          }

          if ( po_language && filesystem->file_exists(*p + "/" + filename) )
          {
            int score = Language::match(language, po_language);
              
            if (score > best_score)
            {
              best_score = score;
              best_filename = filename;
            }
          }
        }
      }

      if (!best_filename.empty())
      {
        std::string pofile = *p + "/" + best_filename;
        try 
        {
          std::unique_ptr<std::istream> in = filesystem->open_file(pofile);
          if (!in.get())
          {
            log_error << "error: failure opening: " << pofile << std::endl;
          }
          else
          {
            dict = new Dictionary(charset,pofile);
            POParser::parse(pofile, *in, *dict);
          }
        }
        catch(std::exception& e) 
        {
          log_error << "error: failure parsing: " << pofile << std::endl;
          log_error << e.what() << "" << std::endl;
        }
      }
    }

    dictionaries[language] = dict;

    return *dict;
  }
}

std::set<Language>
DictionaryManager::get_languages(const std::string& textdomain)
{
  std::set<Language> languages;

  for (SearchPath::iterator p = search_path.begin(); p != search_path.end(); ++p)
  {
    std::vector<std::string> files = std::vector<std::string>();
    std::vector<std::string> dirs = std::vector<std::string>();
    filesystem->open_directory(*p,files,dirs);

    for(std::vector<std::string>::iterator file = files.begin(); file != files.end(); ++file)
    {
      if (has_suffix(*file, ".po") || has_suffix(*file, ".pot"))
      {
        languages.insert(Language::from_env(file->substr(0, file->size()-3)));
      }
    }

    // If we were given a text domain, we also search directories looking for ll_CC/textdomain.po
    if ( !textdomain.empty() )
    {
      for(std::vector<std::string>::iterator directory = dirs.begin(); directory != dirs.end(); directory++)
      {
        Language po_language = Language::from_env(*directory);
        std::string filename = *directory + "/" + textdomain + ".po";

        if (!filesystem->file_exists(*p + "/" + filename))
        {
          filename += "t";
        }

        if ( po_language && filesystem->file_exists(*p + "/" + filename) )
        {
          languages.insert(Language::from_env(*directory));
        }
      }
    }
  }
  return languages;
}

void
DictionaryManager::set_language(const Language& language)
{
  if (current_language != language)
  {
    current_language = language;
    current_dict = 0;
  }
}

Language
DictionaryManager::get_language() const
{
  return current_language;
}

void
DictionaryManager::set_charset(const std::string& charset_)
{
  clear_cache(); // changing charset invalidates cache
  charset = charset_;
}

void
DictionaryManager::set_use_fuzzy(bool t)
{
  clear_cache();
  use_fuzzy = t;
}

bool
DictionaryManager::get_use_fuzzy() const
{
  return use_fuzzy;
}

void
DictionaryManager::add_directory(const std::string& pathname)
{
  clear_cache(); // adding directories invalidates cache
  search_path.push_back(pathname);
}

void
DictionaryManager::set_filesystem(std::unique_ptr<FileSystem> filesystem_)
{
  filesystem = std::move(filesystem_);
}

} // namespace tinygettext

/* EOF */
