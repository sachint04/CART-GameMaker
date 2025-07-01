#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <functional>
#include <stack>


namespace cart{
    
   /* struct PuzzleMakerData {
        int stagewidth = 500;
        int stageheight = 500;
        std::string tmpimg = "treasure-stone.png";
        std::string maskdir = "puzzletemplates/";
        std::string targetdir = "puzzle/";
        std::string templatefile = "ptemplate01.jpg";
        int gridsize = 4;
        int gridFormat = 5;  
        int snapdis = 80;
        std::vector<std::string> masks{	"m1.png", "m2.png","m3.png", "m4.png","m5.png", "m6.png",
            "m7.png","m8.png" ,"m9.png" ,"m10.png" ,"m11.png" , "m12.png",
            "m13.png","m14.png" ,"m15.png" ,"m16.png"  };
    };
    */



    struct DataFile {
        public:
            inline void SetString(const std::string& str, const size_t nItem = 0)
            {
                if (nItem >= m_vContent.size())
                {
                    m_vContent.resize(nItem + 1);
                }
                m_vContent[nItem] = str;
            }
        
            inline const std::string GetString(const size_t nItem = 0) {
                if (nItem >= m_vContent.size())
                {
                    return "";
                }
                return m_vContent[nItem];
            }
        
            inline void SetReal(const double d, const size_t nItem = 0) {
                SetString(std::to_string(d), nItem);
            }
        
            inline const double GetReal(const size_t nItem = 0) {
                return std::atof(GetString(nItem).c_str());
            }
        
            inline void SetInt(const int32_t n, const size_t nItem = 0) {
                SetString(std::to_string(n), nItem);
            }
        
            inline const int32_t GetInt(const size_t nItem = 0) {
                return std::atoi(GetString(nItem).c_str());
            }
        
            inline size_t GetValueCount()const
            {
                return m_vContent.size();
            }

            inline size_t GetVecObjectsCount() {
                return m_vecObjects.size();
            }
            
            inline DataFile& operator[](const std::string& name) {
                // 
                if (m_mapObjects.count(name) == 0)
                {
                    // supplied name doesnt exists in map so add one
                    m_mapObjects[name] = m_vecObjects.size();
                    m_vecObjects.push_back({ name, DataFile() });
                }
               return m_vecObjects[m_mapObjects[name]].second;
            }
        
            // write data file to disk
            inline static bool Write(const DataFile& n, const std::string& sfilename, bool bLastObject = false,  const std::string& sIndent = "\t", const char sListSep = ',')
            {
                std::string sSeparator = std::string(1, sListSep) + " ";
                size_t  nIndentCount = 0;
                std::function<void(const DataFile&, std::ofstream&, bool)> write = [&](const DataFile& n, std::ofstream& file, bool bIsLastNode = false)
                {
                    auto indent = [&](const std::string& sString, const size_t nCount)
                    {
                        std::string sOut;
                        for (size_t i = 0; i < nCount; i++)
                        {
                            sOut += sString;
                        }
                        return sOut;
                    };
                    size_t vecObjectCount = n.m_vecObjects.size();
                    size_t currentVecObject = 0;
                    for (auto property : n.m_vecObjects) {
                        // Does property contains sub objects
                       
                        if (property.second.m_vecObjects.empty())
                        {
                            // No, SO its a assigned field and should be written. If property 
                            // is Flaged as comment, it has not assignment potential. First write
                            // a property name
                           
                            file << indent(sIndent,nIndentCount) <<'"' << property.first << '"' << (property.second.m_bIsComment ? "" : " : ");
                            size_t nItems = property.second.GetValueCount();
                            file << '"';
                            for (size_t i = 0; i < property.second.GetValueCount(); i++)
                            {
                                // if the value is being written, in string form, contains sepration character then
                                // the value must be written in the quotation marks, Note,
                                // if the Value is the last of the list of Values of Property, it is not suffixed with the separator
                                size_t x = property.second.GetString(i).find_first_of(sListSep);
                                if (x != std::string::npos)
                                {
                                    // Value contains separator so wrap it in Quotes
                                    file << "\'" << property.second.GetString(i) << "\'" << ((nItems > 1) ? sSeparator : "");
                                }
                                else {
                                                  
                                    file << property.second.GetString(i) << ((nItems > 1) ? sSeparator : "");
                                }
                                
                                nItems--;
                            }
                            file << '"';       
                            currentVecObject++;
                            if (currentVecObject < vecObjectCount)file << sSeparator;
                            file << "\n";
                        }
                        else {
                            // Yes property has propeties of its own, so its node
                            // force new line and write out nodes name
                            file << "\n" << indent(sIndent, nIndentCount)<< '"' << property.first << '"'<< ":" << '\n';
                            file << indent(sIndent, nIndentCount) << "{\n";
                            nIndentCount++;
                            size_t vecobjsize = property.second.m_vecObjects.size();
                            bool isEnd = (vecobjsize == 0);
                            write(property.second, file, isEnd);
                            file << indent(sIndent, nIndentCount) << "}\n";
                        }
                    }
        
                    // We have finished writing out a node. Regardless of our state or indentation
                    // must decrease, unless we are top level;                
                    if (nIndentCount > 0)nIndentCount--;
                };
                
                std::ofstream file(sfilename);
                if (file.is_open()) {
                    write(n, file, false);
                    return true;
                }
        
                return false;
           }        
            
            
            inline static bool LoadLocalConfig(DataFile& n, const std::string& sfilename, const char sListSep = ',')
            {
                char* strm = LoadFileText(sfilename.c_str());
                if (strm) {
                  
                    DataFile::Read(n, strm);
                    return true;
                }
                return false;
            }


            inline static bool Read(DataFile& n, char* strm, const char sListSep = ',')
            {       
                std::istringstream  file(strm);
               // std::ifstream file(sfilename);
              LOG("Data file data %s", strm);
             //   if (file.is_open())
                if (file)
                {
                    // These value are out side of read loop as  we will
                    // need to refer to previous iteration values in certain conditions
                    std::string sPropName = "";
                    std::string sPropValue = "";
                    std::stack<std::reference_wrapper<DataFile>> stkpath;
                    stkpath.push(n);
                    while (!file.eof()) {
        
                        // read line
                        std::string line;
                        std::getline(file, line);
        
                        // remove white space/ tabs / new line
                        auto trim = [](std::string& s)
                        {
                            s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
                            s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
                        };
        
                        auto trimQuotes = [](std::string& s)
                        {
                            s.erase(0, s.find_first_not_of('"'));
                            s.erase(s.find_last_not_of('"') + 1);                      
                        };
        
                        auto trimlastSeparator = [](std::string& s)
                        {
                            s.erase(s.find_last_not_of(',') + 1);
                        };
        
                        trim(line);
        
                        if (!line.empty())
                        {
                            //Test if it is Comment...
                            if(line[0] == '#')
                            {
                               // It is a comment, so ignore
                               DataFile comment;
                                comment.m_bIsComment = true;
                                stkpath.top().get().m_vecObjects.push_back({ line, comment });
                            }
                            else {
                                size_t x = line.find_first_of(':');
                                // not a end of string
                                if (x != std::string::npos)
                                {
                                    // so split up the property in to its name and value
        
                                    // Extract property name which is all charaters up to first assigment
                                    // trim white space from the end
                                    sPropName = line.substr(0, x);
                                    trim(sPropName);
                                    trimQuotes(sPropName);
                                    // Extract property name which is all charaters up to after the first assigment operator
                                    // trim white space from the end
                                    sPropValue = line.substr(x + 1, line.size());
                                    trim(sPropValue);
                                    trimlastSeparator(sPropValue);
                                    trimQuotes(sPropValue);
        
                                    // Values may in list form: a, b,c,d etc and some elements my exist in quotes a, b ,"c,d", f etc
                                    //so we need to iterate character by character and break up the value;
                                    bool bInQuotes = false;
                                    std::string sToken;
                                    size_t nTokenCount = 0;
                                    for (const auto c : sPropValue)
                                    {
                                        // Is character is in quotes
                                        if (c == '\"')
                                        {
                                            bInQuotes = !bInQuotes;
                                        }
                                        else
                                        {
                                            // No, so we need to proceed  creating a token. if we are in quote state
                                            // then just append character until we exi quote state.
                                            if (bInQuotes)
                                            {
                                                sToken.append(1, c);
                                            }
                                            else
                                            {
                                                // Is the character is a separator? if it is
                                                if (c == sListSep)
                                                {
                                                    // Cleanup token
                                                    trim(sToken);
                                                    // Add it to the vector of values for this property
                                                    stkpath.top().get()[sPropName].SetString(sToken, nTokenCount);
                                                    //Reset our token state
                                                    sToken.clear();
                                                    nTokenCount++;
                                                }
                                                else {
                                                    // it isnt, so just append to token
                                                    sToken.append(1, c);
                                                }
                                            }
                                        }
                                    }
        
                                    // Any residual character at this point just make up  the final token
                                    // so clean it up and add it to the vector of values
                                    if (!sToken.empty())
                                    {
                                        trim(sToken);
                                        stkpath.top().get()[sPropName].SetString(sToken, nTokenCount);
                                    }
                                }
                                // no ' = '
                                else
                                {
                                    if (line[0] == '{')
                                    {
                                        //Open brace, so push node to stack, subsequent node 
                                        // will belongs to new node
                                        stkpath.push(stkpath.top().get()[sPropName]);
        
                                    }
                                    else {
                                        if (line[0] == '}')
                                        {
                                            //Close brace, to this node has been defined, pop it from the
                                            // stack
                                            stkpath.pop();
        
                                        }
                                        else
                                        {
                                            // line is a property with no assignment. who knows whether this is usefull or not
                                            // but we can simpley add it as valueless property
                                            sPropName = line;
                                            //... actually it is useful,  as valueless properties typically are going to be 
                                            // the names of new Data nodes on the next iteration
                                        }
        
                                    }
                                }
                            }
                        }
                    }
        
        
                   // file.close();
                    return true;
                }
        
                return false;
            }
           
        std::vector<std::string> m_vContent;
        std::vector<std::pair<std::string, DataFile>> m_vecObjects;// Actual data with name as first value
        std::unordered_map<std::string, size_t> m_mapObjects;// save name and index in the vector of datafile
         //Used to identify is a property is a comment or not.
            bool m_bIsComment = false;
        };
}