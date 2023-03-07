#include <Windows.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <sstream>




static std::string regex_str = {};

void SetRandomString(char* str) {
    static const char alpha[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    size_t len = strlen(str);


    for (auto i = 4; i < len; ++i) {

        if (str[i] != '@')
        {
            str[i] = alpha[rand() % (sizeof(alpha) - 1)];
        }

    }


}

void PrintDataStrings(const BYTE* data, size_t size) {
    // ��ȡģ��� DOS ͷ
    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)data;
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        std::cerr << "Invalid DOS signature" << std::endl;
        return;
    }

    // ��ȡģ��� NT ͷ
    IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)(data + dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        std::cerr << "Invalid NT signature" << std::endl;
        return;
    }

    // ��ȡģ��� .rdata ��ͷ
    IMAGE_SECTION_HEADER* sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
    for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {
        if (strcmp(reinterpret_cast<const char*>(sectionHeader->Name), ".rdata") == 0) {
            break;
        }
        sectionHeader++;
    }

    // ��ӡ�����е��ַ���
    DWORD offset = sectionHeader->VirtualAddress;
    size -= offset;
    const BYTE* sectionData = data + offset;
    int i = 0;
    while (size > 0) {
	    char* str = (char*)(sectionData);

        if (strlen(str) > 0) {
            std::string v = str;

            //std::regex re("\\?(AV|AU).+@");
         
            std::regex re(regex_str);
            std::smatch match;
            if (std::regex_search(v, match, re)) {

                i++;
                std::cout << i << " | Match found:[" << match[0] << "]" << std::endl;
                //�޸�����ַ���
                SetRandomString(str);
          
           
                std::cout << i << " | Revise:[" << str << + "]" << std::endl;


            }
        }
        size -= strlen(str) + 1;
        sectionData += strlen(str) + 1;
    }
}



int main(int argc, char* argv[])
{
    std::cout << "RTTI obfuscator" << std::endl;

    srand(static_cast<unsigned>(time(nullptr)));
    try
    {
        //������ rtti-obfuscator.exe xxx.dll xxx.rtti.dll "\?(AV|AU).+@"
        if (argc < 3)
        {
            throw std::exception("syntax: %s [pe_file] [input_file] [regex] example xxx.dll xxx.rtti.dll \"""\\?(AV|AU).+@\"");
        }

        //�������������ַ��� ""
        //.?AVbad_alloc@std@@
        //.?AUexception@std@@
        //.?ATexception@std@@

        //���ַ������� "\?(AV|AU|AT).+@"
        //ֻ��һ��AV�Ļ��Ͳ���ҪAU��AT��  "\?(AV).+@"



        // path to input binary
        std::string path = argv[1];
        std::string out_file_name = argv[2];
        regex_str = argv[3];
        std::ifstream fs(path, std::fstream::binary);
        if (fs.fail())
        {
            throw std::exception("Could not open source binary");
        }

        std::stringstream ss;
        ss << fs.rdbuf();
        auto contents = ss.str();
        PrintDataStrings((BYTE*)contents.data(), contents.size());

        size_t pos = path.find_last_of("\\");
        std::string dirPath = {};
   
        if (pos != std::string::npos) {
          
            dirPath = path.substr(0, pos + 1);
        }
        path = dirPath + out_file_name;

        //д���ļ�
        std::ofstream os(path.c_str(), std::ofstream::trunc | std::ofstream::binary);
        if (!os.write(contents.data(), contents.size()))
        {
            throw std::exception((std::string("Unable to write to file ") + path).c_str());
        }
        std::cout << "Successfully obfuscated RTTI information. Output written to " << path << std::endl;
        fs.close();
        os.close();
        std::cin.get();
    }

    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        std::cin.get();
        return 1;
    }

    return 0;
}