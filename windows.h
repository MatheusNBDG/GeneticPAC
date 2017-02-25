#ifndef WINDOWS_H_INCLUDED
#define WINDOWS_H_INCLUDED

#include <windows.h>
#include "wtypes.h"
#include <ole2.h>
#include <olectl.h>
#include "ambient.h"
#include <cmath>
#include <vector>


#define PAC_MAN_SIZE 13

using namespace std;

void move_cusor(const double &_dx,const double &_dy){
    double fScreenWidth    = ::GetSystemMetrics( SM_CXSCREEN )-1;
    double fScreenHeight  = ::GetSystemMetrics( SM_CYSCREEN )-1;
    double fx = _dx*(65535.0f)/fScreenWidth;
    double fy = _dy*(65535.0f)/fScreenHeight;
    INPUT  Input={0};
    Input.type      = INPUT_MOUSE;
    Input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE;
    Input.mi.dx = fx;
    Input.mi.dy = fy;
    ::SendInput(1,&Input,sizeof(INPUT));
}

void left_click()
{
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void press_arrow(const size_t &_arrow)
{
    INPUT input;
    WORD vkey;
    if (_arrow==0) vkey = VK_UP;
    if (_arrow==3) vkey = VK_DOWN;
    if (_arrow==2) vkey = VK_RIGHT;
    if (_arrow==1) vkey = VK_LEFT;
    if (_arrow==4) vkey = VK_ESCAPE;
    if (_arrow==5) vkey = VK_HOME;
    if (_arrow==6) return;
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;
    input.ki.wVk = vkey;
    input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
    SendInput(1, &input, sizeof(INPUT));
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

void color(const size_t &_idx, BYTE* &_bit_pointer, unsigned int &_red, unsigned int &_green, unsigned int &_blue)
{
    _red = _bit_pointer[_idx+2];
    _green = _bit_pointer[_idx+1];
    _blue = _bit_pointer[_idx];
}

pos find_coordinates(const size_t &_screen_width, const size_t &_screen_height, const size_t &_idx)
{
    size_t width4=4*_screen_width;
    pos coordinates(0,0);
    coordinates.x = (_idx % (width4))/4;
    coordinates.y = _screen_height-((_idx-4*coordinates.x)/width4);
    return coordinates;
}

size_t convert_to_idx(const size_t &_screen_width, const size_t &_screen_height, const size_t &_x, const size_t &_y)
{
    return 4*((_screen_height-_y)*_screen_width+_x);
}


vector<pos> initialization(const size_t &_screen_width, const size_t _screen_height)
{
        HDC hdc, hdc_temp;
        BYTE*bit_pointer;
        hdc = GetDC(GetDesktopWindow());
        hdc_temp = CreateCompatibleDC(hdc);
        BITMAPINFO bitmap;
        bitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bitmap.bmiHeader.biWidth = _screen_width;
        bitmap.bmiHeader.biHeight = _screen_height;
        bitmap.bmiHeader.biPlanes = 1;
        bitmap.bmiHeader.biBitCount = 32;
        bitmap.bmiHeader.biCompression = BI_RGB;
        bitmap.bmiHeader.biSizeImage = 0;
        bitmap.bmiHeader.biClrUsed = 0;
        bitmap.bmiHeader.biClrImportant = 0;
        HBITMAP h_bitmap2 = CreateDIBSection(hdc_temp, &bitmap, DIB_RGB_COLORS, (void**)(&bit_pointer), NULL, NULL);
        HGDIOBJ save = SelectObject(hdc_temp, h_bitmap2);
        BitBlt(hdc_temp, 0, 0, _screen_width, _screen_height, hdc, 0, 0, SRCCOPY);
        //return vector
        vector<pos> initial_pos; int k;
        //Open the game
        unsigned int red=0, green=0, blue=0;
        for(size_t j=0;j<_screen_width*_screen_height*4;j+=4)
        {
            color(j,bit_pointer,red,green,blue);
            if(red==255 && green==170 && blue==164){
                pos open_game=find_coordinates(_screen_width,_screen_height,j);
                initial_pos.push_back(open_game);
                move_cusor(open_game.x,open_game.y);
                left_click();
                break;
            }
        }
        Sleep(2300);
        BitBlt(hdc_temp, 0, 0, _screen_width, _screen_height, hdc, 0, 0, SRCCOPY);
        //Find the reference
        for(size_t j=0;j<_screen_width*_screen_height*4;j+=4)
        {
            color(j,bit_pointer,red,green,blue);
            if(red==24 && green==24 && blue==255){
                pos lower_limit=find_coordinates(_screen_width,_screen_height,j);
                move_cusor(lower_limit.x,lower_limit.y);
                initial_pos.push_back(lower_limit);
                break;
            }
        }
        Sleep(500);
        BitBlt(hdc_temp, 0, 0, _screen_width, _screen_height, hdc, 0, 0, SRCCOPY);
        //Find the initial position of pac_mam
        for(size_t j=0;j<_screen_width*_screen_height*4;j+=4)
        {
            color(j,bit_pointer,red,green,blue);
            pos pacman=find_coordinates(_screen_width,_screen_height,j);
            if(red==255 && green==255 && blue==0 && pacman.y<initial_pos[1].y-16){
                move_cusor(pacman.x,pacman.y);
                initial_pos.push_back(pacman);
                break;
            }
        }
        //Delete everything
        SelectObject(hdc_temp, save);
        DeleteObject(save);
        ReleaseDC(GetDesktopWindow(), hdc);
        ReleaseDC(NULL,hdc_temp);
        DeleteDC(hdc);
        DeleteDC(hdc_temp);
        bit_pointer=nullptr;
        delete bit_pointer;
        Sleep(1500);
        return initial_pos;
}

pos find_pacman(BYTE* &_bit_pointer, const size_t &_screen_width,
                const size_t &_screen_height, const size_t &_j_ref)
{
    unsigned int red=0, green=0, blue =0;
    pos pacman(0,0);
    for(size_t j=_j_ref;j<_screen_width*_screen_height*4;j+=4)
        {
            color(j,_bit_pointer,red,green,blue);
            if(red==255 && green==255 && blue==0){
                pos pacman_finder=find_coordinates(_screen_width,_screen_height,j);
                size_t yellow=9;
                for(int i=-1;i!=2;i++)
                    for(int k=-1;k!=2;k++){
                        size_t possible_j=convert_to_idx(_screen_width,_screen_height,pacman_finder.x+k,pacman_finder.y+i);
                        color(possible_j,_bit_pointer,red,green,blue);
                        if(red!=255 || green!=255 || blue!=0)
                            if(!i||!k) yellow--;
                            else yellow=0; // I have put it, maybe give some problem!/
                    }
                if(yellow==8){
                    for(int i=-1;i!=2;i++)
                        for(int k=-1;k!=2;k++){
                            size_t possible_j=convert_to_idx(_screen_width,_screen_height,pacman_finder.x+k,pacman_finder.y+i);
                            color(possible_j,_bit_pointer,red,green,blue);
                            if(red!=255 || green!=255 || blue!=0)
                                pacman_finder.x+=3*k;
                                pacman_finder.y+=3*i;
                                break;
                        }
                    if(pacman_finder.x<_screen_width&&pacman_finder.y<_screen_height)
                        pacman=pacman_finder;
                    move_cusor(pacman.x,pacman.y);
                    return pacman;
            }
        }
    }
    return pacman;
}

Ambient look_at_direction(const pos &_pacman, const bool &_act_x,const bool &_act_y, const int &_direction,
                          const size_t &_screen_width, const size_t &_screen_height, BYTE* &_bit_pointer)
{
    unsigned int reward=0;
    unsigned int red=0, green=0, blue=0;
    for(size_t i=0; i!=_screen_width ; i++)
        for(int p=-4; p!=5; p++){
            size_t position = convert_to_idx(_screen_width, _screen_height,
                                             _pacman.x+_direction*_act_x*i+_act_y*p,
                                             _pacman.y+_direction*_act_y*i+_act_x*p);
            color(position,_bit_pointer, red, green, blue);
            if(red==248&&green==176&&blue==144) reward++;
            if(red!=248||green!=176||blue!=144)
                if(red!=0||green!=0||blue!=0)
                    if(red!=255||green!=255||blue!=0){
                        Ambient limit(i,red,green,blue,reward);
                        return limit;
                    }
                    else{
                       size_t Count=0;
                       for(int f=-1;f!=2;f++)
                            for(int g=-1; g!=2;g++){
                                size_t position = convert_to_idx(_screen_width, _screen_height,
                                                                 _pacman.x+_direction*_act_x*i+_act_y*p+f,
                                                                 _pacman.y+_direction*_act_y*i+_act_x*p+g);
                                color(position,_bit_pointer, red, green, blue);
                                if(red==255&&green==255&&blue==0) Count++;
                       }
                       if(Count==3){
                            size_t position = convert_to_idx(_screen_width, _screen_height,
                                                             _pacman.x+_direction*_act_x*i+_act_y*p,
                                                             _pacman.y+_direction*_act_y*i+_act_x*p);
                            color(position,_bit_pointer, red, green, blue);
                            Ambient limit(i,red,green,blue,reward);
                            return limit;
                       }
                    }
        }
}

class vector_map{
public:
    vector_map(const pos &_position,const int _direction, const unsigned int &_reward):
        position(_position), direction(_direction), reward(_reward) {}
    pos position;
    int direction;
    unsigned reward;
private:
};

vector<vector_map> map_for_scapes( pos &_pacman, const bool &_act_x,const bool &_act_y, const int &_direction,
                                  const size_t &_screen_width, const size_t &_screen_height, BYTE* &_bit_pointer)
{
    vector<vector_map> first_scapes;
    size_t mim_i_up=13, mim_i_down=13;
    unsigned int reward=0;
    int direction=0;
    unsigned int red=0, green=0, blue=0;
    for(size_t i=0; i<_screen_width; i++){
        //Looking reward
        for(int p=-6; p!=7; p++){
            size_t position = convert_to_idx(_screen_width, _screen_height,
                                             _pacman.x+_direction*_act_x*i+_act_y*p,
                                             _pacman.y+_direction*_act_y*i+_act_x*p);
            color(position,_bit_pointer, red, green, blue);
            if(red==248&&green==176&&blue==144) reward++;
        }
        //Up
        if(i>mim_i_up)
            for(size_t s=1; s<_screen_width ; s++){
                size_t position = convert_to_idx(_screen_width, _screen_height,
                                                _pacman.x+_direction*_act_x*i+_act_y*s,
                                                _pacman.y+_direction*_act_y*i+_act_x*s);
                color(position,_bit_pointer, red, green, blue);
                if(red!=248||green!=176||blue!=144)
                    if(red!=0||green!=0||blue!=0)
                        if(s<21)
                            if(red==255&&green==255&&blue==0&&s<11) ;
                            else break;
                        else{
                            pos initial(_pacman.x+_direction*_act_x*(i+5),_pacman.y+_direction*_act_y*(i+5));
                            direction=1*_act_x+2*_act_y;
                            vector_map map_vec(initial,direction,reward);
                            first_scapes.push_back(map_vec);
                            mim_i_up=i+30;
                            break;
                        }
            }
        //Down
        if(i>mim_i_down)
            for(int s=-1;s>-_screen_width ; s--){
                size_t position = convert_to_idx(_screen_width, _screen_height,
                                                _pacman.x+_direction*_act_x*i+_act_y*s,
                                                _pacman.y+_direction*_act_y*i+_act_x*s);
                color(position,_bit_pointer, red, green, blue);
                if(red!=248||green!=176||blue!=144)
                    if(red!=0||green!=0||blue!=0)
                        if(s>-21)
                            if(red==255&&green==255&&blue==0&&s>-11) ;
                            else break;
                        else{
                            pos initial(_pacman.x+_direction*_act_x*(i+5),_pacman.y+_direction*_act_y*(i+5));
                            direction=-1*_act_x-2*_act_y;
                            vector_map map_vec(initial,direction,reward);
                            first_scapes.push_back(map_vec);
                            mim_i_down=i+30;
                            break;
                        }
            }
        bool control=0;
        for(int p=-4; p!=5; p++){
            size_t position = convert_to_idx(_screen_width, _screen_height,
                                             _pacman.x+_direction*_act_x*i+_act_y*p,
                                             _pacman.y+_direction*_act_y*i+_act_x*p);
            color(position,_bit_pointer, red, green, blue);
            if(red!=248||green!=176||blue!=144)
                if(red!=0||green!=0||blue!=0)
                    if(red!=255||green!=255||blue!=0){
                        control=1;
                        break;
                    }
                    else{
                       size_t Count=0;
                       for(int f=-1;f!=2;f++)
                            for(int g=-1; g!=2;g++){
                                size_t position = convert_to_idx(_screen_width, _screen_height,
                                                                 _pacman.x+_direction*_act_x*i+_act_y*p+f,
                                                                 _pacman.y+_direction*_act_y*i+_act_x*p+g);
                                color(position,_bit_pointer, red, green, blue);
                                if(red==255&&green==255&&blue==0) Count++;
                       }
                       if(Count==3){
                            control=1;
                            break;
                       }
                    }
        }
        if(control) break;
    }
    return first_scapes;
}


vector<Ambient> scan_direction(pos &_pacman, int &_direction, BYTE* &_bit_pointer,
                               const size_t &_screen_width, const size_t &_screen_height)
{
    bool act_x=0, act_y=0;
    int direction=1;
    if(abs(_direction)==1) act_x=1;
    if(abs(_direction)==2) act_y=1;
    if(_direction<0) direction= -1;
    vector<vector_map> direction_map = map_for_scapes(_pacman, act_x, act_y, direction,
                                                      _screen_width, _screen_height, _bit_pointer);
    vector<Ambient> amb_vec;
    Ambient amb_mem=look_at_direction(_pacman, act_x, act_y, direction,
                                      _screen_width, _screen_height, _bit_pointer);
    amb_vec.push_back(amb_mem);
    for(size_t i=0; i!=direction_map.size(); i++){
        act_x=1; act_y=1; direction=1;
        if(abs(direction_map[i].direction)==1) act_x=0;
        if(abs(direction_map[i].direction)==2) act_y=0;
        if(direction_map[i].direction<0) direction= -1;
        vector<vector_map> direction_map_2 = map_for_scapes(direction_map[i].position, act_x, act_y, direction,
                                                            _screen_width, _screen_height, _bit_pointer);
        Ambient amb_mem=look_at_direction(direction_map[i].position, act_x, act_y, direction,
                                          _screen_width, _screen_height, _bit_pointer);
        amb_vec.push_back(amb_mem);
        amb_vec.back().d_2=abs(direction_map[i].position.x-_pacman.x+direction_map[i].position.y-_pacman.y);
        amb_vec.back().reward_2=direction_map[i].reward;
        for(size_t k=0; k!=direction_map_2.size(); k++){
            act_x=1; act_y=1; direction=1;
            if(abs(direction_map_2[k].direction)==1) act_x=0;
            if(abs(direction_map_2[k].direction)==2) act_y=0;
            if(direction_map_2[k].direction<0) direction= -1;
            amb_mem = look_at_direction(direction_map_2[k].position, act_x, act_y, direction,
                                        _screen_width, _screen_height, _bit_pointer);
            amb_mem.d_3=abs(direction_map_2[k].position.x-direction_map[i].position.x+direction_map_2[k].position.y-direction_map[i].position.y);
            amb_mem.d_2=abs(direction_map[i].position.x-_pacman.x+direction_map[i].position.y-_pacman.y);
            amb_mem.reward_3=direction_map_2[k].reward;
            amb_mem.reward_2=direction_map[i].reward;
            amb_vec.push_back(amb_mem);
        }
    }
    return amb_vec;
}


void get_score(pos &_pacman, vector<pos> &_store_pos,  BYTE* &_bit_pointer,
               const size_t &_screen_width, const size_t &_screen_height,const size_t &_j_ref)
{
    size_t score=0;
    size_t j_mem=0;
    unsigned int red=0, green=0, blue=0;
    for(size_t j=_j_ref;j<=_screen_width*_screen_height*4;j+=4)
    {
        if(score==0) j_mem=j;
        color(j, _bit_pointer, red,green, blue);
        pos mem(0,0);
        mem=find_coordinates(_screen_width,_screen_height,j);
        if(red==222&&green==222&&blue==255)
        {
            std::vector<int> up_limit ={3,2,3,3,8,3,7,4,2,2}; //maybe can be sensitive to screen resolution
            std::vector<int> right_limit={4,7,8,6,3,6,6,3,6,5};
            size_t control=0;
            for(size_t n=0;n!=10;n++){
                control=0;
                for(size_t i=0;i!=up_limit[n]+1;i++){
                    size_t position = convert_to_idx(_screen_width,_screen_height, mem.x,mem.y-i);
                    color(position, _bit_pointer, red,green,blue);
                    if(red==0&&green==0&&blue==0)
                        if(i==up_limit[n]-1)
                            control ++;
                        else break;
                }
                for(size_t i=0;i!=right_limit[n]+1;i++){
                    size_t position = convert_to_idx(_screen_width,_screen_height, mem.x+i,mem.y);
                    color(position, _bit_pointer, red,green,blue);
                    if(red==0&&green==0&&blue==0)
                        if(i==right_limit[n]-1) control ++;
                        else break;
                }
                if (control==2){
                    if(n==3){
                        size_t position = convert_to_idx(_screen_width,_screen_height, mem.x,mem.y-5);
                        // maybe be sensitive to screen resolution
                        color(position, _bit_pointer, red,green,blue);
                        if(red==0&&green==0&&blue==0) n=3;
                        else n=5;
                    }
                    score=10*score+n;
                    j=4*((_screen_height-mem.y)*_screen_width+mem.x+right_limit[n]-1); //-1
                    j_mem=j;
                    break;
                }
            }
            if(j-j_mem>4*7) break; // DEBUG: para que serve j mem mesmo?
        }
    }
    _store_pos.back().y=_store_pos.back().x;
    _store_pos.back().x=score;
}

vector<vector<Ambient>> scan_ambient(pos &_pacman, vector<pos> &_store_pos,
                                     const size_t &_screen_width, const size_t &_screen_height)
{
    // Initialization of variables
    vector<vector<Ambient>> ambient;
    _pacman.x=0;
    HDC hdc=NULL, hdc_temp=NULL;
    BYTE*bit_pointer;
    while(hdc==NULL)
        hdc = GetDC(GetDesktopWindow());
    hdc_temp = CreateCompatibleDC(hdc);
    BITMAPINFO bitmap;
    // Configuration of BitMap
    bitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap.bmiHeader.biWidth = _screen_width;
    bitmap.bmiHeader.biHeight = _screen_height;
    bitmap.bmiHeader.biPlanes = 1;
    bitmap.bmiHeader.biBitCount = 32;
    bitmap.bmiHeader.biCompression = BI_RGB;
    bitmap.bmiHeader.biSizeImage = 0;
    bitmap.bmiHeader.biClrUsed = 0;
    bitmap.bmiHeader.biClrImportant = 0;
    HBITMAP h_bitmap2 = CreateDIBSection(hdc_temp, &bitmap, DIB_RGB_COLORS, (void**)(&bit_pointer), NULL, NULL);
    if (hdc_temp != NULL)
    {
        HGDIOBJ save = SelectObject(hdc_temp, h_bitmap2);
        BitBlt(hdc_temp, 0, 0, _screen_width, _screen_height, hdc, 0, 0, SRCCOPY);
        size_t x_ref=_store_pos[1].x;
        size_t y_ref=_store_pos[1].y;
        size_t j_ref=convert_to_idx(_screen_width,_screen_height, x_ref, y_ref);
        // Find pacman
        _pacman=find_pacman(bit_pointer, _screen_width, _screen_height, j_ref);
        if(_pacman.x==0){ // if not found
            SelectObject(hdc_temp,save); //delete everything
            DeleteObject(h_bitmap2);
            ReleaseDC(GetDesktopWindow(), hdc);
            DeleteDC(hdc);
            DeleteDC(hdc_temp);
            bit_pointer=nullptr;
            delete bit_pointer;
            Ambient amb_elm(0,0,0,0,0);
            vector<Ambient> amb = {amb_elm};
            ambient.push_back(amb);
            return ambient; // return a null ambient
        }
        // Look the ambient
        for(int i=-2;i!=3;i++)
            if(i) ambient.push_back(scan_direction(_pacman, i, bit_pointer, _screen_width, _screen_height));
        //get score
        get_score(_pacman, _store_pos, bit_pointer, _screen_width, _screen_height, j_ref);
        //Delete everything
        SelectObject(hdc_temp,save);
        DeleteObject(h_bitmap2);
        ReleaseDC(GetDesktopWindow(), hdc);
        DeleteDC(hdc);
        DeleteDC(hdc_temp);
        bit_pointer=nullptr;
        delete bit_pointer;
    }
    return ambient;
}

#endif // WINDOWS_H_INCLUDED
