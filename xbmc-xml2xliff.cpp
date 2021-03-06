/*
 *      Copyright (C) 2005-2008 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#ifdef TIXML_USE_STL
  #include <iostream>
  #include <sstream>
  using namespace std;
#else
  #include <stdio.h>
#endif

#if defined( WIN32 ) && defined( TUNE )
  #include <crtdbg.h>
  _CrtMemState startMemState;
  _CrtMemState endMemState;
#endif

#include "tinyxml.h"
#include "iostream"
#include <map>
#include <string>

  
int main(int argc, char* argv[])
{
  // char strInputFilname[1024] = argv[1];
  // char strOutputFilname[1024] = argv[2];
  char* pInputFilename = argv[1];
  char* pOutputFilename = argv[2];

  TiXmlDocument xmlDocInput;
  if (!xmlDocInput.LoadFile(pInputFilename))
  {
    std::cout << xmlDocInput.ErrorDesc() << " " << pInputFilename << std::endl;
    return 1;
  }

  // Check if we really have "strings" root element and we have at least one child element
  TiXmlElement* pRootElementInput = xmlDocInput.RootElement();
  if (!pRootElementInput || pRootElementInput->NoChildren() ||
       pRootElementInput->ValueTStr()!="strings")
  {
    std::cout << "error: No root element called: strings or no child found in input xml" << std::endl;
    return 1;
  }

  std::multimap<std::string, int> mapXmlData;

  const TiXmlElement *pChildInput = pRootElementInput->FirstChildElement("string");
  const char* pAttrIdInput = NULL;
  const char* pValue = NULL;
  while (pChildInput)
  {
    pAttrIdInput=pChildInput->Attribute("id");
    if (pAttrIdInput && !pChildInput->NoChildren())
    {
      int id = atoi(pAttrIdInput);
      pValue = pChildInput->FirstChild()->Value();
      mapXmlData.insert(std::pair<std::string,int>( pValue,id));
    }
    pChildInput = pChildInput->NextSiblingElement("string");
  }

  // Initalize the output xml document
  TiXmlDocument xmlDocOutput;
  // Create header declaration line
  TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
  xmlDocOutput.LinkEndChild(decl);
  // Create comment
  TiXmlComment * comment = new TiXmlComment();
  comment->SetValue(" Converted from xbmc strings.xml file " );
  xmlDocOutput.LinkEndChild(comment);
  //create node called xliff
  TiXmlElement* nodexliff = new TiXmlElement("xliff");
  xmlDocOutput.LinkEndChild(nodexliff);
  nodexliff->SetAttribute("version", "1.1");
  //create node called file
  TiXmlElement* nodefile = new TiXmlElement("file");
  nodexliff->LinkEndChild(nodefile);
  nodefile->SetAttribute("original", "strings.xml");
  nodefile->SetAttribute("source-language", "en-US");
  nodefile->SetAttribute("datatype", "plaintext");
  //create node called body
  TiXmlElement* nodebody = new TiXmlElement("body");
  nodefile->LinkEndChild(nodebody);

  pChildInput = pRootElementInput->FirstChildElement("string");
  pAttrIdInput = NULL;
  pValue = NULL;
  int previd = -1;
  while (pChildInput)
  {
    pAttrIdInput=pChildInput->Attribute("id");
    if (pAttrIdInput && !pChildInput->NoChildren())
    {
      int id=atoi(pAttrIdInput);
      pValue = pChildInput->FirstChild()->Value();

      //create comment note if empty string or strings ids found
      if (previd !=-1 && (id-previd >= 2))
      {
        TiXmlComment * comment = new TiXmlComment();
        char stremptycomment[64];
        if (id-previd == 2) sprintf(stremptycomment," Empty string with id %i ", id-1);
        if (id-previd > 2) sprintf(stremptycomment," Empty strings from id %i to %i ", previd+1, id-1);
        comment->SetValue(stremptycomment);
        nodebody->LinkEndChild(comment);
      }

      //create node trans-unit
      TiXmlElement* nodetransunit = new TiXmlElement("trans-unit");
      nodebody->LinkEndChild(nodetransunit);
      nodetransunit->SetAttribute("id", pAttrIdInput);
      //create node source with value
      TiXmlElement* nodesource = new TiXmlElement("source");
      nodesource->LinkEndChild(new TiXmlText( pValue));
      nodetransunit->LinkEndChild(nodesource);
      // if we have multiple IDs for the same string value, we create a context node
      if (mapXmlData.count( pValue) > 1)	// if we have multiple IDs for the same string value
      {
        //create node context-group id with value
        TiXmlElement* nodecontextgroup = new TiXmlElement("context-group");
        TiXmlElement* nodecontext = new TiXmlElement("context");
        nodecontext->SetAttribute("context-type", "context");
        char strContext[64];
        sprintf(strContext,"Auto generated context for string id %i", id);
        nodecontext->LinkEndChild(new TiXmlText(strContext));
        nodecontextgroup->LinkEndChild(nodecontext);
        nodetransunit->LinkEndChild(nodecontextgroup);
      }
      previd =id;
    }
    pChildInput = pChildInput->NextSiblingElement("string");
  }

  // Save the created output file
  if (!xmlDocOutput.SaveFile(pOutputFilename))
  {
    std::cout << xmlDocOutput.ErrorDesc() << " " << pOutputFilename << std::endl;
    return 1;
  }

  // Free up the allocated memory
  xmlDocInput.Clear();
  xmlDocOutput.Clear();

//std::cout << "success" << xmlDocInput.ErrorDesc(); 
//  for(int i = 1; i < argc; i++)
//      std::cout << "argv[" << i << "] = " << argv[i] << std::endl; 
  return 0;
}
