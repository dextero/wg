#ifndef RAPIDXML_UTILS_HPP_INCLUDED
#define RAPIDXML_UTILS_HPP_INCLUDED

// Copyright (C) 2006, 2008 Marcin Kalicinski
// Version 1.12
// Revision $DateTime: 2008/11/01 17:40:11 $
//! \file rapidxml_utils.hpp This file contains high-level rapidxml utilities that can be useful
//! in certain simple scenarios. They should probably not be used if maximizing performance is the main objective.

#include "rapidxml.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

namespace rapidxml
{

    //! Represents data loaded from a file
    template<class Ch = char>
    class file
    {
        
    public:
        
        //! Loads file into the memory. Data will be automatically destroyed by the destructor.
        //! \param filename Filename to load.
        file(const char *filename)
        {
            using namespace std;

            // Open stream
			FILE *stream = fopen(filename, "rb");
            if (!stream)
                throw runtime_error(string("cannot open file ") + filename);
//            stream.unsetf(ios::skipws);
            
            // Determine stream size
		    fseek(stream, 0, SEEK_END);
			size_t size = ftell(stream);
			fseek(stream, 0, SEEK_SET);      
            
            // Load data and add terminating 0
            m_data.resize(size + 1);
            fread(&m_data.front(), 1, size, stream);
            m_data[size] = 0;

            fclose(stream);
        }

        //! Loads file into the memory. Data will be automatically destroyed by the destructor
        //! \param stream Stream to load from
        file(std::basic_istream<Ch> &stream)
        {
            using namespace std;

            // Load data and add terminating 0
            stream.unsetf(ios::skipws);
            m_data.assign(istreambuf_iterator<Ch>(stream), istreambuf_iterator<Ch>());
            if (stream.fail() || stream.bad())
                throw runtime_error("error reading stream");
            m_data.push_back(0);
        }
        
        //! Gets file data.
        //! \return Pointer to data of file.
        Ch *data()
        {
            return &m_data.front();
        }

        //! Gets file data.
        //! \return Pointer to data of file.
        const Ch *data() const
        {
            return &m_data.front();
        }

        //! Gets file data size.
        //! \return Size of file data, in characters.
        std::size_t size() const
        {
            return m_data.size();
        }

    private:

        std::vector<Ch> m_data;   // File data

    };

    //! Counts children of node. Time complexity is O(n).
    //! \return Number of children of node
    template<class Ch>
    inline std::size_t count_children(xml_node<Ch> *node)
    {
        xml_node<Ch> *child = node->first_node();
        std::size_t count = 0;
        while (child)
        {
            ++count;
            child = child->next_sibling();
        }
        return count;
    }

    //! Counts attributes of node. Time complexity is O(n).
    //! \return Number of attributes of node
    template<class Ch>
    inline std::size_t count_attributes(xml_node<Ch> *node)
    {
        xml_attribute<Ch> *attr = node->first_attribute();
        std::size_t count = 0;
        while (attr)
        {
            ++count;
            attr = attr->next_attribute();
        }
        return count;
    }

}

#endif
