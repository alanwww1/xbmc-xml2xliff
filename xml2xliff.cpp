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

  // Initalize the output xml document
  TiXmlDocument xmlDocOutput;
  // Create header declaration line
  TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
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

  const TiXmlElement *pChildInput = pRootElementInput->FirstChildElement("string");
  const char* attrIdInput = NULL;
  while (pChildInput)
  {
    attrIdInput=pChildInput->Attribute("id");
    if (attrIdInput && !pChildInput->NoChildren())
    {
      //create node trans-unit
      TiXmlElement* nodetransunit = new TiXmlElement("trans-unit");
      nodefile->LinkEndChild(nodetransunit);
      nodetransunit->SetAttribute("id", attrIdInput);
      //create node context-group id with value
      TiXmlElement* nodecontextid = new TiXmlElement("context-group");
      nodecontextid->SetAttribute("context-type", "id");
      nodecontextid->LinkEndChild(new TiXmlText(attrIdInput));
      nodetransunit->LinkEndChild(nodecontextid);
      //create node source with value
      TiXmlElement* nodesource = new TiXmlElement("source");
      nodesource->LinkEndChild(new TiXmlText(pChildInput->FirstChild()->Value()));
      nodetransunit->LinkEndChild(nodesource);
    }
    pChildInput = pChildInput->NextSiblingElement("string");
  }

  // Save the created output file
  if (!xmlDocOutput.SaveFile(pOutputFilename))
  {
    std::cout << xmlDocOutput.ErrorDesc() << " " << pOutputFilename << std::endl;
    return 1;
  }

//std::cout << "success" << xmlDocInput.ErrorDesc(); 
//  for(int i = 1; i < argc; i++)
//      std::cout << "argv[" << i << "] = " << argv[i] << std::endl; 
  return 0;
}
