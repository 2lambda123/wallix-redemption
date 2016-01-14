/*
 * 
 */


#include <iostream>
#include <fstream>
#include <sstream>
#include "../src/keyboard/keylayouts.hpp"
//#include "keymap2.hpp"
#define SIZE 128


void tabToReversedMap(const Keylayout::KeyLayout_t & read, std::ofstream & fichier, std::string name);


static const Keylayout * keylayouts[] = { &keylayout_x00000405, &keylayout_x00000406, &keylayout_x00000407
                                            , &keylayout_x00000408, &keylayout_x00000409, &keylayout_x0000040a
                                            , &keylayout_x0000040b, &keylayout_x0000040c, &keylayout_x0000040f
                                            , &keylayout_x00000410, &keylayout_x00000413, &keylayout_x00000414
                                            , &keylayout_x00000415, &keylayout_x00000416, &keylayout_x00000418
                                            , &keylayout_x00000419, &keylayout_x0000041a, &keylayout_x0000041b
                                            , &keylayout_x0000041d, &keylayout_x0000041f, &keylayout_x00000422
                                            , &keylayout_x00000424, &keylayout_x00000425, &keylayout_x00000426
                                            , &keylayout_x00000427, &keylayout_x0000042f, &keylayout_x00000438
                                            , &keylayout_x0000043a, &keylayout_x0000043b, &keylayout_x0000043f
                                            , &keylayout_x00000440, &keylayout_x00000444, &keylayout_x00000450
                                            , &keylayout_x00000452, &keylayout_x0000046e, &keylayout_x00000481
                                            , &keylayout_x00000807, &keylayout_x00000809, &keylayout_x0000080a
                                            , &keylayout_x0000080c, &keylayout_x00000813, &keylayout_x00000816
                                            , &keylayout_x0000081a, &keylayout_x0000083b, &keylayout_x00000843
                                            , &keylayout_x0000085d, &keylayout_x00000c0c, &keylayout_x00000c1a
                                            , &keylayout_x00001009, &keylayout_x0000100c, &keylayout_x0000201a
                                            , &keylayout_x00010402, &keylayout_x00010405, &keylayout_x00001809
                                            , &keylayout_x00010407, &keylayout_x00010408, &keylayout_x0001040a
                                            , &keylayout_x0001040e, &keylayout_x00010409, &keylayout_x00010410
                                            , &keylayout_x00010415, &keylayout_x00010416, &keylayout_x00010419
                                            , &keylayout_x0001041b, &keylayout_x0001041f, &keylayout_x00010426
                                            , &keylayout_x00010427, &keylayout_x0001043a, &keylayout_x0001043b
                                            , &keylayout_x0001080c, &keylayout_x0001083b, &keylayout_x00011009
                                            , &keylayout_x00011809, &keylayout_x00020405, &keylayout_x00020408
                                            , &keylayout_x00020409, &keylayout_x0002083b, &keylayout_x00030402
                                            , &keylayout_x00030408, &keylayout_x00030409, &keylayout_x00040408
                                            , &keylayout_x00040409, &keylayout_x00050408, &keylayout_x00060408
                                            };  
                                            
static int over(0);
    
    
int main () {
    
        
    
    for(int i = 0; i < 84; i++) {
        
        const Keylayout *  keylayout_WORK(keylayouts[i]); 
        int LCID(keylayout_WORK->LCID);
        int LCIDreverse(LCID+0x80000000);
        
        std::stringstream ss;
        ss << std::hex << LCIDreverse;
        std::string name = ss.str();
        std::ofstream fichier(("src/keyboard/reversed_keymaps/keylayout_x"+name+".hpp").c_str(), std::ios::out | std::ios::trunc);
        
        if(fichier) {

            fichier << "#ifndef _REDEMPTION_KEYBOARD_KEYLAYOUT_R_X"<<std::hex<<LCIDreverse<<"_HPP_" << std::endl;
            fichier << "#define _REDEMPTION_KEYBOARD_KEYLAYOUT_R_X"<<std::hex<<LCIDreverse<<"_HPP_" << std::endl<< std::endl;
            
            fichier << "#include \"keylayout_r.hpp\"" << std::endl<< std::endl;
            
            fichier << "namespace x"<< std::hex<<LCIDreverse << "{ " << std::endl<< std::endl;
            
            fichier << "const static int LCID = 0x"<< std::hex<<LCIDreverse <<";"<< std::endl<< std::endl;

            fichier << "const static char * const locale_name = \""<<keylayout_WORK->locale_name<<"\";"<< std::endl<< std::endl;

            tabToReversedMap(keylayout_WORK->noMod,               fichier,  "noMod");
            tabToReversedMap(keylayout_WORK->shift,               fichier,  "shift");
            tabToReversedMap(keylayout_WORK->altGr,               fichier,  "altGr");
            tabToReversedMap(keylayout_WORK->shiftAltGr,          fichier,  "shiftAltGr");
            tabToReversedMap(keylayout_WORK->capslock_noMod,      fichier,  "capslock_noMod");
            tabToReversedMap(keylayout_WORK->capslock_shift,      fichier,  "capslock_shift");
            tabToReversedMap(keylayout_WORK->capslock_altGr,      fichier,  "capslock_altGr");
            tabToReversedMap(keylayout_WORK->capslock_shiftAltGr, fichier,  "capslock_shiftAltGr");
            tabToReversedMap(keylayout_WORK->ctrl,                fichier,  "ctrl");
            
            fichier << "const Keylayout_r::KeyLayoutMap_t deadkeys" << std::endl;
            fichier << "{" << std::endl;
            for (int i = 0; i < keylayout_WORK->nbDeadkeys; i++) {
                int deadCode(keylayout_WORK->deadkeys[i].extendedKeyCode);
                fichier << "\t{ 0x" << std::hex << keylayout_WORK->deadkeys[i].uchar << ", 0x" << deadCode << "}," << std::endl;
                int nbSecondDK(keylayout_WORK->deadkeys[i].nbSecondKeys);
                for (int j = 0; j < nbSecondDK; j++) {
                    fichier << "\t{ 0x" << std::hex << keylayout_WORK->deadkeys[i].secondKeys[j].secondKey << ", 0x" << keylayout_WORK->deadkeys[i].secondKeys[j].modifiedKey << "}," << std::endl;
                }
            }
            fichier << "};" << std::endl;
            fichier << std::endl << std::endl;
            
           /* fichier << "const Keylayout_r::dkey_t deadkeys[] = {" << std::endl;
            fichier << "\t{ 0, 0,  0, { {0, 0} " << std::endl;
            fichier << "\t            }" << std::endl;
            fichier << "\t}," << std::endl;
            fichier << "};" << std::endl << std::endl;*/
            
           int nbDeadKeys(keylayout_WORK->nbDeadkeys);
            fichier << "const static uint8_t nbDeadkeys = " << std::dec << nbDeadKeys << ";" << std::endl << std::endl;
            
            fichier << "}" << std::endl << std::endl;
            
            fichier << "static const Keylayout_r keylayout_x"<< std::hex<<LCIDreverse <<"( x"<< std::hex<<LCIDreverse <<"::LCID" << std::endl;
            fichier << "                                 , x"<< std::hex<<LCIDreverse <<"::locale_name" << std::endl;
            fichier << "                                 , x"<< std::hex<<LCIDreverse <<"::noMod" << std::endl;
            fichier << "                                 , x"<< std::hex<<LCIDreverse <<"::shift" << std::endl;
            fichier << "                                 , x"<< std::hex<<LCIDreverse <<"::altGr" << std::endl;
            fichier << "                                 , x"<< std::hex<<LCIDreverse <<"::shiftAltGr" << std::endl;
            fichier << "                                 , x"<< std::hex<<LCIDreverse <<"::ctrl" << std::endl;
            fichier << "                                 , x"<< std::hex<<LCIDreverse <<"::capslock_noMod" << std::endl;
            fichier << "                                 , x"<< std::hex<<LCIDreverse <<"::capslock_shift" << std::endl;
            fichier << "                                 , x"<< std::hex<<LCIDreverse <<"::capslock_altGr" << std::endl;
            fichier << "                                 , x"<< std::hex<<LCIDreverse <<"::capslock_shiftAltGr" << std::endl;
            fichier << "                                 , x"<< std::hex<<LCIDreverse <<"::deadkeys" << std::endl;
            fichier << "                                 , x"<< std::hex<<LCIDreverse <<"::nbDeadkeys" << std::endl << std::endl;
            fichier << ");" << std::endl << std::endl;

            fichier << "#endif" << std::endl;
            
            fichier.close();  

        } else {

            std::cerr << "Error file" << std::endl;

        }
    }
    
    std::cout << "missed data = " << over << std::endl;
    
    return 0;
}



void tabToReversedMap(const Keylayout::KeyLayout_t & read, std::ofstream & fichier, std::string name) {
    fichier << "const Keylayout_r::KeyLayoutMap_t "<< name << std::endl;
    fichier << "{" << std::endl;
    for(int i = 0; i < SIZE; i++) {
        if (read[i] != 0){
            if (read[i] < 16) {
                fichier << "\t{ " << "0x000" << read[i] << ", 0x" << i << " }," << std::endl;
            } else if (read[i] < 256){
                fichier << "\t{ " << "0x00" << read[i] << ", 0x" << i << " }," << std::endl;
            } else if (read[i] < 256*16) {
                fichier << "\t{ " << "0x0" << read[i] << ", 0x" << i << " }," << std::endl;
            } else if (read[i] < 256*256) {
                fichier << "\t{ " << "0x" << read[i] << ", 0x" << i << " }," << std::endl;
            } else {
                over++;
            }
        }
    }
    fichier << "};" << std::endl;
    
    fichier << std::endl << std::endl;
}



void printDeadKeys(int send[]){
    
}