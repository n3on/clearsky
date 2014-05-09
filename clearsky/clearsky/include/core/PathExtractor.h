/*
*
*/

#include "core/precompiled.h"

#pragma once
#ifndef _CLEARSKY_PATH_EXTRACTOR_H_
#define _CLEARSKY_PATH_EXTRACTOR_H_

namespace clearsky
{
	template<typename charType>
	class PathExtractor;
	typedef PathExtractor<char> StringPathExtractor;

	template <typename charType>
	class PathExtractor
	{
	private:
	std::basic_string<charType> m_string;
	charType m_delimiter;

	public:
		/*
		* c:\long\evil\gay\unleet\fileName.ext
		********************************************/

		// Retrieves: c:\long\evil\gay\unleet
		// Empty if filename only
		inline const std::basic_string<charType> extractFilePath() const
		{
			std::basic_string<charType> tmp;
			size_t pos = m_string.rfind(m_delimiter);
			if(pos == m_string.npos)
			{
				return tmp; 
			}
			tmp.assign(m_string, 0, pos);
			return tmp;
		}

		// Retrieves for maxTier=3:
		// \evil\gay\unleet\filename.ext
		inline const std::basic_string<charType> extractFilePathExt(const unsigned int maxTier) const
		{
			std::basic_string<charType> tmp;
			if (m_string.empty())
			{
				return tmp;
			}

			// Always valid:
			size_t pos = m_string.size()-1;
			unsigned int i = 0;

			// While not at front and maxTier not achieved:
			while (pos != 0 && i < maxTier)
			{
				if (m_string[pos] == m_delimiter)
				{
					// Increase current tier count:
					i++;
				}
				// Move backwards:
				pos--;
			}
			tmp = m_string.substr(pos);
			return tmp;
		}

		// Retrieves: ext
		inline const std::basic_string<charType> extractFileExt() const
		{
			std::basic_string<charType> tmp;
			size_t pos = m_string.rfind('.');
			if(pos != m_string.npos)
			{
				tmp.assign(m_string.substr(pos));
			}
			return tmp;
		}

		/* Retrieves: c:\
		*/
		inline const std::basic_string<charType> extractHomeDir() const
		{
			std::basic_string<charType> tmp;
			size_t pos = m_string.find(m_delimiter);
			if(pos != m_string.npos)
			{
				tmp.assign(m_string.substr(0, pos));
			}
			return tmp;
		}


		// changes current string
		inline void setString(const std::basic_string<charType>& str)
		{
			m_string = str;
		}

		// extracts file name without path
		inline const std::basic_string<charType> extractFileName() const
		{
			std::basic_string<charType> tmp;
			size_t pos = m_string.rfind(m_delimiter);
			if(pos != m_string.npos)
			{
				tmp.assign(m_string.substr(pos+1));
			}
			return tmp;
		}


		// Transforms m_string into a relative path to string "to":
		inline const std::basic_string<charType> makeRelative(const std::basic_string<charType>& pivot)
		{
			// Step 1: Get base dir from pivot path:
			PathExtractor<charType> x(pivot, m_delimiter);

			// Step 2: Compare with local base dir,
			// return full path if different
			if (extractHomeDir() != x.extractHomeDir())
			{
				return m_string;
			}

			// Same root directory, gather common base path:
			unsigned int maxPos = pivot.size()<=m_string.size()?pivot.size():m_string.size();

			unsigned int currPos = 0;
			// As long as both strings are the same, go forward
			while (currPos < maxPos && m_string[currPos] == pivot[currPos])
			{
				currPos++;
			}

			// Tada, newRoot now countains the common base path
			std::basic_string<charType> relativePath;
			
			// see what we have now:	m_string	...target
			//							pivot		...pivot path
			//							currPos		...last common position

			// now go upwards to the cbp till it is reached:
			unsigned int bu = currPos;
			for (; currPos < pivot.size(); currPos++)
			{
				if (pivot[currPos] == m_delimiter)
				{
					// we go upwards, add ..<delimiter>
					relativePath += ".." + m_delimiter;
				}
			}

			// now relativePath has taken us to the cbp, we still need to go downwards to the target:

			//append remainder of current path:
			relativePath += m_string.substr(bu);


			// Done.
			return relativePath;
		}

		// Evil ctor
		inline PathExtractor(std::basic_string<charType> str, charType delim):
							m_string(str),m_delimiter(delim)
		{
			// Evil idle
		}
	};

}

#endif