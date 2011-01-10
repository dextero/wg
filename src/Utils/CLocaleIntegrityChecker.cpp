#include "CLocaleIntegrityChecker.h"
#include "CXml.h"
#include "StringUtils.h"
#include <iostream>
#include <algorithm>
#include <boost/filesystem.hpp>

bool CLocaleIntegrityChecker::Check(const std::vector<std::string>& langs)
{
    std::cerr << "--- Performing locale check ---\n";

    // wyciagamy wszystkie locale
    // wszystko z data/locale/[jezyk], rekurencyjnie
    std::vector<std::string> dirs;
    std::vector<std::vector<std::string> > langFiles;

    for (size_t i = 0; i < langs.size(); ++i)
    {
        if (!boost::filesystem::exists("data/locale/" + langs[i]))
            std::cerr << "WARNING: language '" << langs[i] << "' not found!\n";
        else
        {
            langFiles.push_back(std::vector<std::string>());
	        dirs.push_back("data/locale/" + langs[i]);

	        while (dirs.size() > 0)
            {
		        std::string dir = dirs[dirs.size()-1];
		        dirs.pop_back();

		        boost::filesystem::directory_iterator di(dir),dir_end;
		        for (; di != dir_end; di++){
			        if (di->leaf() == ".svn")
				        continue;
			        std::string file = dir + "/" + di->leaf();
			        if (boost::filesystem::is_directory(file))
				        dirs.push_back(file);
			        else
				        langFiles[i].push_back(file);
		        }
	        }
        }

        std::sort(langFiles[i].begin(), langFiles[i].end());
    }

    unsigned int missingLocaleFiles = 0, filesWithMissingKeys = 0, minFiles = 0;
    if (langFiles.size() == 0)
        std::cerr << "ERROR: no languages selected!\n";
    else
        minFiles = langFiles[0].size();

    // koniec ladowania locale
    // sprawdzamy, czy sa wszystkie pliki:
    for (size_t lang = 1; lang < langFiles.size(); ++lang)
    {
        minFiles = std::min((size_t)minFiles, langFiles[lang].size());

        std::cerr << "Comparing '" << langs[0] << "' with '" << langs[lang] << "'\n";
        unsigned int leftIter = 0, rightIter = 0;
        while (leftIter < langFiles[0].size() && rightIter < langFiles[lang].size())
        {
            std::string leftFile = langFiles[0][leftIter].substr(langFiles[0][leftIter].find('/', std::string("data/locale/").size())),
                        rightFile = langFiles[lang][rightIter].substr(langFiles[lang][rightIter].find('/', std::string("data/locale/").size()));

            // directory_iterator leci alfabetycznie, wiec jesli ktorakolwiek para nie bedzie sie zgadzac, to czegos brakuje
            if (leftFile > rightFile)
            {
                // brakuje czegos po lewej
                std::cerr << "Language: " << langs[0] << " - missing file: " << rightFile << std::endl;
                ++leftIter;
                ++missingLocaleFiles;
            }
            else if (leftFile < rightFile)
            {
                // brakuje czegos po prawej
                std::cerr << "Language: " << langs[lang] << " - missing file: " << leftFile << std::endl;
                ++rightIter;
                ++missingLocaleFiles;
            }
            else
            {
                filesWithMissingKeys += Check(langFiles[0][leftIter], langFiles[lang][rightIter]);
                ++leftIter;
                ++rightIter;
            }
        }
    }

    // byly w ogole jakies pliki?
    if (minFiles == 0)
        std::cerr << "ERROR: no files found in 1 or more languages!\n";

    std::cerr << "\n- missing files: " << missingLocaleFiles << "\n- files with missing keys: " << filesWithMissingKeys << "\n--- Locale check completed ---\n";

    return (missingLocaleFiles != 0 || filesWithMissingKeys != 0 || minFiles == 0);
}


bool CLocaleIntegrityChecker::Check(
    const std::string& leftFile, const std::string& rightFile)
{
    bool areExtras = false;
    Dictionary left, right;
    Load(leftFile, left);
    Load(rightFile, right);

    // obcinam data/locale, bo niepotrzebnie rozszerza na boki...
    std::cerr <<  "Comparing '" << leftFile.substr(12)
              << "' with '" << rightFile.substr(12) << "'... ";

    Dictionary::iterator leftIt = left.begin();
    Dictionary::iterator rightIt = right.begin();
    while(leftIt != left.end() && rightIt != right.end())
    {
        if (leftIt->first == rightIt->first)
        { ++leftIt; ++rightIt; }
        else if (leftIt->first > rightIt->first)
        {
            std::cerr << std::endl << "missing " << rightIt->second.key;
            areExtras = true;
            ++rightIt;
        }
        else
        {
            std::cerr << std::endl << "extra " << leftIt->second.key;
            areExtras = true;
            ++leftIt;
        }
    }
    if (areExtras)
        std::cerr << std::endl;
    std::cerr << std::max(left.size(), right.size()) 
        << " entries" << (areExtras ? "" : " OK!") << std::endl;
    return areExtras;
}

bool CLocaleIntegrityChecker::Load(
    const std::string & filename, Dictionary & dictionary)
{
    CXml xml(filename, "root");
    if (xml.GetString(xml.GetRootNode(),"type") != "locale")
        return false;

    Entry entry;
    for (rapidxml::xml_node<> *n = xml.GetChild(xml.GetRootNode(),"entry"); n; n = xml.GetSibl(n,"entry"))
    {
        entry.key = xml.GetString(xml.GetChild(n,"key"));
        entry.value = StringUtils::ReinterpretFromUTF8(xml.GetString(xml.GetChild(n,"value")));

        dictionary[entry.key] = entry;
    }
    return true;
}
