#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include "chmat.h"
#include <vector>
#define sgn(x) ((x<0)?-1:((x>0)?1:0)) /* macro to return the sign of a number */
int FillBMPInfo(BITMAPINFO* info, int x, int y, int bits)
{
    info->bmiHeader.biBitCount = bits;
    info->bmiHeader.biClrImportant = 0;
    info->bmiHeader.biClrUsed = 0;
    info->bmiHeader.biCompression = BI_RGB;
    info->bmiHeader.biHeight = y;
    info->bmiHeader.biPlanes = 1;
    info->bmiHeader.biSize = sizeof(BITMAPINFO);
    info->bmiHeader.biSizeImage = x * y * bits / 8;
    info->bmiHeader.biWidth = x;
    info->bmiHeader.biXPelsPerMeter = 0;
    info->bmiHeader.biYPelsPerMeter = 0;
    return(0);
}


CHMAT::CHMAT(int x, int y, int typ)
{
    if (typ == CHMAT_UNSIGNED_CHAR)
        data = calloc(x * y, sizeof(char));
    else if (typ == CHMAT_INT || typ == CHMAT_UNSIGNED_INT)
    {
        BITMAPINFO binfo;
        FillBMPInfo(&binfo, x, y, 32);
        HBitmap = CreateDIBSection(NULL, &binfo, DIB_RGB_COLORS, (void**)&data, NULL, NULL);
    }
    if (data != NULL)
    {
        X = x;
        Y = y;
        type = typ;
    }
}

CHMAT::~CHMAT() 
{ 
    if (type == CHMAT_INT)
        DeleteObject(HBitmap);
    else free(data);
}



void CHMAT::print_mat()
{
    int x, y;
    unsigned char* ucd = (unsigned char*)data;
    int* id = (int*)data;
    for (y = 0; y < Y; y++)
    {
        for (x = 0; x < X; x++)
        {
            if (type == CHMAT_UNSIGNED_CHAR)
                printf("%d ", ucd[y * X + x]);
            else if (type == CHMAT_INT)
                printf("%d ", id[y * X + x]);
        }
        printf("\n");
    }
    printf("\n");
}

void CHMAT::operator = (int a)
{
    int x, y;
    unsigned char* ucd = (unsigned char*)data;
    int* id = (int*)data;
    for (y = 0; y < Y; y++)
    {
        for (x = 0; x < X; x++)
        {
            if (type == CHMAT_UNSIGNED_CHAR)
                ucd[y * X + x] = a;
            else if (type == CHMAT_INT)
                id[y * X + x] = a;
        }       
    }
}


template<typename T>
void CHMAT::operator += (T a)
{
    int x, y;
    unsigned char* ucd = (unsigned char*)data;
    int* id = (int*)data;
    unsigned int* uid = (unsigned int*)data;
    double* dd = (double*)data;

    for (y = 0; y < Y; y++) {
        for (x = 0; x < X; x++) {
            if (type == CHMAT_UNSIGNED_CHAR)
                ucd[y * X + x] += (unsigned char)a;
            else if (type == CHMAT_INT)
                id[y * X + x] += (int)a;
            else if (type == CHMAT_UNSIGNED_INT)
                uid[y * X + x] += (unsigned int)a;
            else if (type == CHMAT_DOUBLE)
                dd[y * X + x] += (double)a;
        }
    }
}

template<typename T>
void CHMAT::operator /= (T a)
{
    int x, y;
    unsigned char* ucd = (unsigned char*)data;
    int* id = (int*)data;
    unsigned int* uid = (unsigned int*)data;
    float* fd = (float*)data;
    double* dd = (double*)data;

    for (y = 0; y < Y; y++) {
        for (x = 0; x < X; x++) {
            if (type == CHMAT_UNSIGNED_CHAR)
                ucd[y * X + x] /= (unsigned char)a;
            else if (type == CHMAT_INT)
                id[y * X + x] /= (int)a;
            else if (type == CHMAT_UNSIGNED_INT)
                uid[y * X + x] /= (unsigned int)a;
            else if (type == CHMAT_DOUBLE)
                dd[y * X + x] /= (double)a;
        }
    }
}

void CHMAT::operator *= (CHMAT& a)
{
    if (X != a.Y) return;
    if (a.type != type)return;

    CHMAT m(a.X, Y, type);// create an empty matris to hold the result
    //mat_mult(m, a);//calculate the multiplication and put it in m
    //mat_equal(m);// self = m

    return;
}
void CHMAT::mat_mult(CHMAT& m, CHMAT& a) {//ismet

    if (type == CHMAT_UNSIGNED_CHAR) {
        unsigned char* ucd = (unsigned char*)data;
        unsigned char* ucd3 = (unsigned char*)a.data;
        int val;

        for (int x = 0; x < a.X; x++) {
            for (int y = 0; y < Y; y++) {
                val = 0;
                for (int i = 0; i < X; i++) {
                    val += ucd[(y) * 0 + x + i] * ucd3[(0 + i) * X + x];// 1st. matrix row * 2nd. matrix column, then add each one
                }
                m.set(x + 1, y + 1, val);// set the value to m
            }
        }
    }
    else if (type == CHMAT_INT) {
        int* id = (int*)data;
        int* id3 = (int*)a.data;
        int val;

        for (int x = 0; x < a.X; x++) {
            for (int y = 0; y < Y; y++) {
                val = 0;
                for (int i = 0; i < X; i++) {
                    val += id[(y) * 0 + x + i] * id3[(0 + i) * X + x];
                }
                m.set(x + 1, y + 1, val);
            }
        }
    }
    else if (type == CHMAT_UNSIGNED_INT) {
        unsigned int* uid = (unsigned int*)data;
        unsigned int* uid3 = (unsigned int*)a.data;
        int val;

        for (int x = 0; x < a.X; x++) {
            for (int y = 0; y < Y; y++) {
                val = 0;
                for (int i = 0; i < X; i++) {
                    val += uid[(y) * 0 + x + i] * uid3[(0 + i) * X + x];
                }
                m.set(x + 1, y + 1, val);
            }
        }
    }
    else if (type == CHMAT_DOUBLE) {
        double* dd = (double*)data;
        double* dd3 = (double*)a.data;
        double val;

        for (int x = 0; x < a.X; x++) {
            for (int y = 0; y < Y; y++) {
                val = 0;
                for (int i = 0; i < X; i++) {
                    val += dd[(y) * 0 + x + i] * dd3[(0 + i) * X + x];
                }
                m.set(x + 1, y + 1, val);
            }
        }
    }
}
void CHMAT::mat_subtr(CHMAT& m, CHMAT& a)//ismet
{
    if (type == CHMAT_UNSIGNED_CHAR)
    {
        unsigned char* ucd = (unsigned char*)data;
        unsigned char* ucd2 = (unsigned char*)a.data;
        int val;

        for (int x = 0; x < a.X; x++)
        {
            for (int y = 0; y < Y; y++)
            {
                val = ucd[(y)*X + x] - ucd2[y * X + x];

                m.set(x + 1, y + 1, val);
            }
        }
    }

    else if (type == CHMAT_INT)
    {
        int* id = (int*)data;
        int* id2 = (int*)a.data;
        int val;

        for (int x = 0; x < a.X; x++)
        {
            for (int y = 0; y < Y; y++)
            {

                val = id[(y)*X + x] - id2[y * X + x];
                m.set(x + 1, y + 1, val);
            }
        }
    }

    else if (type == CHMAT_UNSIGNED_INT)
    {
        unsigned int* uid = (unsigned int*)data;
        unsigned int* uid2 = (unsigned int*)a.data;
        int val;

        for (int x = 0; x < a.X; x++)
        {
            for (int y = 0; y < Y; y++)
            {
                val = uid[(y)*X + x] - uid2[y * X + x];
                m.set(x + 1, y + 1, val);
            }
        }
    }

    else if (type == CHMAT_FLOAT)
    {
        float* fd = (float*)data;
        float* fd2 = (float*)a.data;
        float val;

        for (int x = 0; x < a.X; x++)
        {
            for (int y = 0; y < Y; y++)
            {
                val = fd[(y)*X + x] - fd2[y * X + x];

                m.set(x + 1, y + 1, val);
            }
        }
    }

    else if (type == CHMAT_DOUBLE)
    {
        double* dd = (double*)data;
        double* dd2 = (double*)a.data;
        double val;

        for (int x = 0; x < a.X; x++)
        {
            for (int y = 0; y < Y; y++)
            {
                val = 0;
                val = dd[(y)*X + x] - dd2[y * X + x];

                m.set(x + 1, y + 1, val);
            }
        }
    }
}



void CHMAT::mat_add(CHMAT& m, CHMAT& a)//ismet
{
    if (type == CHMAT_UNSIGNED_CHAR)
    {
        unsigned char* ucd = (unsigned char*)data;
        unsigned char* ucd2 = (unsigned char*)a.data;
        int val;

        for (int x = 0; x < a.X; x++)
        {
            for (int y = 0; y < Y; y++)
            {
                val = ucd[(y)*X + x] + ucd2[y * X + x];

                m.set(x + 1, y + 1, val);
            }
        }
    }

    else if (type == CHMAT_INT)
    {
        int* id = (int*)data;
        int* id2 = (int*)a.data;
        int val;

        for (int x = 0; x < a.X; x++)
        {
            for (int y = 0; y < Y; y++)
            {

                val = id[(y)*X + x] + id2[y * X + x];
                m.set(x + 1, y + 1, val);
            }
        }
    }

    else if (type == CHMAT_UNSIGNED_INT)
    {
        unsigned int* uid = (unsigned int*)data;
        unsigned int* uid2 = (unsigned int*)a.data;
        int val;

        for (int x = 0; x < a.X; x++)
        {
            for (int y = 0; y < Y; y++)
            {
                val = uid[(y)*X + x] + uid2[y * X + x];
                m.set(x + 1, y + 1, val);
            }
        }
    }

    else if (type == CHMAT_FLOAT)
    {
        float* fd = (float*)data;
        float* fd2 = (float*)a.data;
        float val;

        for (int x = 0; x < a.X; x++)
        {
            for (int y = 0; y < Y; y++)
            {
                val = fd[(y)*X + x] + fd2[y * X + x];

                m.set(x + 1, y + 1, val);
            }
        }
    }

    else if (type == CHMAT_DOUBLE)
    {
        double* dd = (double*)data;
        double* dd2 = (double*)a.data;
        double val;

        for (int x = 0; x < a.X; x++)
        {
            for (int y = 0; y < Y; y++)
            {
                val = 0;
                val = dd[(y)*X + x] + dd2[y * X + x];

                m.set(x + 1, y + 1, val);
            }
        }
    }
}


int& CHMAT::operator () (int x, int y)
{
    static int r = 0;
    if (x > X || y > Y); //???????????????????
    int* t = (int*)data;
    return t[X * (y - 1) + (x - 1)];
}

template<typename T>int CHMAT::set(int x, int y, T val)
{
    if (x<1 || x>X) return (-1);
    if (y<1 || y>Y) return (-1);

    if (type == CHMAT_UNSIGNED_CHAR)
        ((unsigned char*)data)[(y - 1) * X + (x - 1)] = val;
    else if (type == CHMAT_INT)
        ((int*)data)[(y - 1) * X + (x - 1)] = val;
    else if (type == CHMAT_UNSIGNED_INT)
        ((unsigned int*)data)[(y - 1) * X + (x - 1)] = val;
    else if (type == CHMAT_DOUBLE)
        ((double*)data)[(y - 1) * X + (x - 1)] = (double)val;
    return 0;
}
template int CHMAT::set<int>(int, int, int);
template int CHMAT::set<double>(int, int, double);

bool Rectangle(CHMAT &target,int x1, int y1, int width, int height, int color, bool is_filled)
{
    if (x1 > 1020|| y1 > 700)
    {
        return false;
    }

    int i, j, k, l;

    if (is_filled == true)
    {
        for (j = y1; j <= y1 + height; j++)
        {
            for (i = x1; i <= x1 + width; i++)
            {
                target.set(i, j, color);
            }
        }
    }
    else if (is_filled == false)
    {
        for (i = x1; i <= x1 + width; i++)
        {
            target.set(i, y1, color);
        }

        for (j = y1; j <= y1 + height; j++)
        {
            target.set(x1, j, color);
        }

        for (k = x1 + width; k >= x1; k--)
        {
            target.set(k, y1 + height, color);
        }

        for (l = y1 + height; l >= y1; l--)
        {
            target.set(x1 + width, l, color);
        }
    }
    return true;
}

struct cord
{
    int x = 0, y = 0;
};

cord cuttingPoint(cord A, cord B, cord C, cord D) {//ismet


    // Line AB represented as a1x + b1y = c1 
    double a1 = B.y - A.y;
    double b1 = A.x - B.x;
    double c1 = a1 * (A.x) + b1 * (A.y);

    // Line CD represented as a2x + b2y = c2 
    double a2 = D.y - C.y;
    double b2 = C.x - D.x;
    double c2 = a2 * (C.x) + b2 * (C.y);
    cord result;

    double determinant = a1 * b2 - a2 * b1;

    if (determinant == 0)
    {
        // The lines are parallel. This is simplified 
        // by returning a pair of FLT_MAX 
        return result;
    }
    else
    {
        double x = (b2 * c1 - b1 * c2) / determinant;
        double y = (a1 * c2 - a2 * c1) / determinant;
        result.x = x;
        result.y = y;
        return result;
    }
}
void Line(CHMAT& target, int x1, int y1, int x2, int y2, int color) {//abdulaziz
    int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;

    dx = x2 - x1;      /* the horizontal distance of the line */
    dy = y2 - y1;      /* the vertical distance of the line */
    dxabs = abs(dx);
    dyabs = abs(dy);
    sdx = sgn(dx);
    sdy = sgn(dy);
    x = dyabs >> 1;
    y = dxabs >> 1;
    px = x1;
    py = y1;

    if (dxabs >= dyabs) /* the line is more horizontal than vertical */
    {
        for (i = 0; i < dxabs; i++)
        {
            y += dyabs;
            if (y >= dxabs)
            {
                y -= dxabs;
                py += sdy;
            }
            px += sdx;
            target.set(px, py, color);
        }
    }
    else /* the line is more vertical than horizontal */
    {
        for (i = 0; i < dyabs; i++)
        {
            x += dxabs;
            if (x >= dyabs)
            {
                x -= dyabs;
                px += sdx;
            }
            py += sdy;
            target.set(px, py, color);
        }
    }
    target.set(x1, y1, color);
    target.set(x2, y2, color);

}


bool Ellipse(CHMAT& target,int x, int y, int width, int height, int col, bool isFill)
{
    cord A, B, D, O, holder, h1, h2;
    bool result = false;

    A.x = x;
    A.y = y + height / 2;

    B.x = x + width / 2;
    B.y = y + height;

    D.x = x + width / 2;
    D.y = y;

    O.x = x + width / 2;
    O.y = y + height / 2;


    int counter1 = 0, counter2 = 0, pre_point_x = A.x, pre_point_y = A.y;
    int xlen = width / 2, ylen = height / 2;
    int counter = ylen;

    if (xlen > ylen) counter = xlen;



    for (int i = 0; xlen >= counter1 || ylen >= counter2; i++)
    {

        h1.x = x + counter1;
        h1.y = O.y;

        h2.x = x;
        h2.y = O.y + counter2;

        float pers1 = (counter1 * 100 / xlen);
        float pers2 = (counter2 * 100 / ylen);


        float difference = pers1 - pers2;
        if (difference < 0) difference = difference * (-1);

        if (pers1 == pers2) {
            counter1++;
            counter2++;
        }
        else if (pers1 > pers2) {
            counter2++;
            continue;
        }
        else {
            counter1++;
            continue;
        }
        holder = cuttingPoint(D, h1, B, h2);

        if (target.set(holder.x, holder.y, col) == 0) result = true;
        Line(target,holder.x, holder.y, pre_point_x, pre_point_y, col);

        if (target.set(holder.x + 2 * (O.x - holder.x), holder.y, col) == 0) result = true;
        Line(target, holder.x + 2 * (O.x - holder.x), holder.y, pre_point_x + 2 * (O.x - pre_point_x), pre_point_y, col);


        if (target.set(holder.x, holder.y - 2 * (holder.y - O.y), col) == 0) result = true;
        Line(target, holder.x, holder.y - 2 * (holder.y - O.y), pre_point_x, pre_point_y - 2 * (pre_point_y - O.y), col);

        if (target.set(holder.x + 2 * (O.x - holder.x), holder.y - 2 * (holder.y - O.y), col) == 0) result = true;
        Line(target, holder.x + 2 * (O.x - holder.x), holder.y - 2 * (holder.y - O.y), pre_point_x + 2 * (O.x - pre_point_x), pre_point_y - 2 * (pre_point_y - O.y), col);


        if (isFill) {
            if (pre_point_y != holder.y)Line_for_ellipse(target,pre_point_x, pre_point_y, holder.x, holder.y, col, true, O.x, O.y, width, height);

            for (int j = 0; j < O.x - holder.x; j++)
            {
                target.set(holder.x + j, holder.y, col);

                target.set(O.x + j, holder.y, col);

                target.set(holder.x + j, holder.y - 2 * (holder.y - O.y), col);
                target.set(O.x + j, holder.y - 2 * (holder.y - O.y), col);

            }
        }
        pre_point_x = holder.x;
        pre_point_y = holder.y;

    }

    // this part is for testing the ellipse inside of the given rectangle
    /*for (int i = 0; i < width; i++) set(x +i, y, 0xff0f);
    for (int i = 0; i < height; i++) set(x , y+i, 0xff0f);
    for (int i = height; i > 0; i--) set(x+width, y+i, 0xff0f);
    for (int i = width; i > 0; i--) set(x + i, y + height, 0xff0f);*/
    return result;
}

void Line_for_ellipse(CHMAT& target,int x1, int y1, int x2, int y2, int color, bool isFill, int o_x, int o_y, int width, int height) {//ismet
    int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;

    dx = x2 - x1;      /* the horizontal distance of the line */
    dy = y2 - y1;      /* the vertical distance of the line */
    dxabs = abs(dx);
    dyabs = abs(dy);
    sdx = sgn(dx);
    sdy = sgn(dy);
    x = dyabs >> 1;
    y = dxabs >> 1;
    px = x1;
    py = y1;

    if (dxabs >= dyabs) /* the line is more horizontal than vertical */
    {
        for (i = 0; i < dxabs; i++)
        {
            y += dyabs;
            if (y >= dxabs)
            {
                y -= dxabs;
                py += sdy;
            }
            px += sdx;
            if (isFill) {
                for (int i = 0; i < o_x - x1; i++)
                {
                    target.set(px + i, py, color);
                    target.set(o_x + o_x - (px + i), py, color);
                    target.set(px + i, (o_y - (py - o_y)), color);
                    target.set(o_x + o_x - (px + i), (o_y - (py - o_y)), color);

                }
            }
            else target.set(px, py, color);
        }
    }
    else /* the line is more vertical than horizontal */
    {
        for (i = 0; i < dyabs; i++)
        {
            x += dxabs;
            if (x >= dyabs)
            {
                x -= dyabs;
                px += sdx;
            }
            py += sdy;
            if (isFill) {
                for (int i = 0; i < o_x - x1; i++)
                {
                    target.set(px + i, py, color);



                    target.set(o_x + o_x - (px + i), py, color);
                    target.set(px + i, (o_y - (py - o_y)), color);
                    target.set(o_x + o_x - (px + i), (o_y - (py - o_y)), color);
                }
            }
            else target.set(px, py, color);
        }
    }
}

void CHMAT::operator += (CHMAT& a)//-- abdulaziz
{
    if (X != a.Y) return;
    if (a.type != type)return;

    CHMAT m(a.X, Y, type);// create an empty matris to hold the result
    mat_add(m, a);//calculate the multiplication and put it in m
    mat_equal(m);// self = m

    return;
}

void CHMAT::operator -= (int a)//--abdulaziz

{
    int x, y;
    int type = getType();
    unsigned char* ucd = (unsigned char*)data;
    int* id = (int*)data;
    unsigned int* uid = (unsigned int*)data;
    double* dd = (double*)data;
    for (y = 0; y < Y; y++)
    {
        for (x = 0; x < X; x++)
        {
            if (type == CHMAT_UNSIGNED_CHAR)

                ucd[y * X + x] -= a;
            else if (type == CHMAT_INT)
                id[y * X + x] -= a;
            else if (type == CHMAT_UNSIGNED_INT)
                uid[y * X + x] -= a;
            else if (type == CHMAT_DOUBLE)
                dd[y * X + x] -= a;
            else {
                // give type error
            }
        }
    }
}



void CHMAT::operator -= (double a)//--abdulaziz

{
    int x, y;
    unsigned char* ucd = (unsigned char*)data;
    int* id = (int*)data;
    unsigned int* uid = (unsigned int*)data;
    double* dd = (double*)data;
    for (y = 0; y < Y; y++)
    {
        for (x = 0; x < X; x++)
        {
            if (type == CHMAT_UNSIGNED_CHAR)

                ucd[y * X + x] -= a;
            else if (type == CHMAT_INT)
                id[y * X + x] -= a;
            else if (type == CHMAT_UNSIGNED_INT)
                uid[y * X + x] -= a;
            else if (type == CHMAT_DOUBLE)
                dd[y * X + x] -= a;
            else {
                // give type error
            }
        }
    }
}



//  geters and setters -------------------------

// getters
int CHMAT::getX() {//--abdulaziz

    return X;
}

int CHMAT::getY() {//--abdulaziz

    return Y;
}

int CHMAT::getType() {//--abdulaziz

    return type;
}

void* CHMAT::getData() {//--abdulaziz

    return data;
}

// setters
void CHMAT::setY(int y) {//--abdulaziz

    Y = y;
}



//---- ilhan

//Author: Ilhan
void CHMAT::operator *= (int a)
{
    int x, y;
    unsigned char* ucd = (unsigned char*)data;
    int* id = (int*)data;

    for (y = 0; y < Y; y++)
    {
        for (x = 0; x < X; x++)
        {
            if (type == CHMAT_UNSIGNED_CHAR)
                ucd[y * X + x] *= a;
            else if (type == CHMAT_INT)
                id[y * X + x] *= a;
        }
    }
}

//Author: Ilhan
void CHMAT::operator *= (double a)
{
    int x, y;
    unsigned char* ucd = (unsigned char*)data;
    unsigned int* uid = (unsigned int*)data;
    int* id = (int*)data;
    double* dd = (double*)data;

    for (y = 0; y < Y; y++)
    {
        for (x = 0; x < X; x++)
        {
            if (type == CHMAT_UNSIGNED_CHAR)
                ucd[y * X + x] *= a;
            else if (type == CHMAT_INT)
                id[y * X + x] *= a;
            else if (type == CHMAT_DOUBLE)
                dd[y * X + x] *= a;
        }
    }
}




//Author: Ilhan
void CHMAT::operator -= (CHMAT& a)
{
    if (X != a.X || Y != a.Y)
    {
        printf("Invalid values!");
        return;
    }

    if (a.type != type)
    {
        printf("Type mismatch!");
        return;
    }

    CHMAT m(X, Y, type);
    mat_subtr(m, a);
    mat_equal(m);
}



template void CHMAT::operator /= <int>(int);
template void CHMAT::operator /= <double>(double);
template void CHMAT::operator += <int>(int);
template void CHMAT::operator += <double>(double);

template int CHMAT::set<int>(int, int, int);
template int CHMAT::set<double>(int, int, double);


/*
CHMAT& CHMAT::operator * (CHMAT &a)
{
    int* id  = (int*)data;
    int* id2 = (int*)a.data;
    int val;

    CHMAT m( a.X, Y, CHMAT_INT);
    int* id3 = (int*)m.data;

    if (X != a.Y) return m;

    for (int x = 0; x < m.X; x++)
    {
        for (int y = 0; y < m.Y; y++)
        {
            val = 0;
            for (int i = 0; i < X; i++)
            {
                val += id[(y) * 0 + x+i] * id2[(0+i) * X + x];

            }

            m.set(x + 1, y + 1, val);
        }
    }

    return m;
}
*/
void CHMAT::mat_equal(CHMAT& m) {//ismet


    free(data);

    unsigned char* ucd = (unsigned char*)m.data;
    int* id = (int*)m.data;
    unsigned int* uid = (unsigned int*)m.data;
    double* dd = (double*)m.data;

    X = m.X;
    Y = m.Y;
    type = m.type;

    if (m.type == CHMAT_UNSIGNED_CHAR) {
        data = calloc(X * Y, sizeof(unsigned char));
        if (data == NULL)
            return;

        for (int y = 0; y < m.Y; y++) {
            for (int x = 0; x < m.X; x++) {
                set(x + 1, y + 1, ucd[y * X + x]);
            }
        }
    }
    else if (m.type == CHMAT_INT) {
        data = calloc(X * Y, sizeof(int));
        if (data == NULL)
            return;

        for (int y = 0; y < m.Y; y++) {
            for (int x = 0; x < m.X; x++) {
                set(x + 1, y + 1, id[y * X + x]);
            }
        }
    }
    else if (m.type == CHMAT_UNSIGNED_INT) {
        data = calloc(X * Y, sizeof(unsigned int));
        if (data == NULL)
            return;

        for (int y = 0; y < m.Y; y++) {
            for (int x = 0; x < m.X; x++) {
                set(x + 1, y + 1, uid[y * X + x]);
            }
        }
    }
    else if (m.type == CHMAT_DOUBLE) {
        data = calloc(X * Y, sizeof(double));
        if (data == NULL)
            return;

        for (int y = 0; y < m.Y; y++) {
            for (int x = 0; x < m.X; x++) {
                set(x + 1, y + 1, dd[y * X + x]);
            }
        }
    }
}
void CHMAT::printnumber(int x0, int y0, int number, int c, int b) {

    //std::cout << "I AM HERE!!!!!";
    std::vector<std::vector<int>> mergeM;
    switch (number)
    {
    case 0:       
        mergeM = {
        {	b, b, c, c, c, c, c, c, c, c, b, b  },
        {	b, c, c, c, c, c, c, c, c, c, c, b  },
        {	c, c, c, c, b, b, b, b, c, c, c, c  },
        {	c, c, c, c, b, b, b, b, c, c, c, c  },
        {	c, c, c, b, b, b, b, b, b, c, c, c  },
        {	c, c, c, b, b, b, b, b, b, c, c, c  },
        {	c, c, c, b, b, b, b, b, b, c, c, c  },
        {	c, c, c, b, b, b, b, b, b, c, c, c  },
        {	c, c, c, c, b, b, b, b, c, c, c, c  },
        {	c, c, c, c, b, b, b, b, c, c, c, c  },
        {	b, c, c, c, c, c, c, c, c, c, c, b  },
        {	b, b, c, c, c, c, c, c, c, c, b, b  },
        };
        break;
    case 1:
        mergeM = {
        {	b, b, b, b, c, c, c, c, b, b, b, b  },
        {	b, b, b, c, c, c, c, c, b, b, b, b  },
        {	b, b, c, c, c, c, c, c, b, b, b, b  },
        {	b, c, c, b, c, c, c, c, b, b, b, b  },
        {	c, c, c, b, c, c, c, c, b, b, b, b  },
        {	b, b, b, b, c, c, c, c, b, b, b, b  },
        {	b, b, b, b, c, c, c, c, b, b, b, b  },
        {	b, b, b, b, c, c, c, c, b, b, b, b  },
        {	b, b, b, b, c, c, c, c, b, b, b, b  },
        {	b, b, b, b, c, c, c, c, b, b, b, b  },
        {	c, c, c, c, c, c, c, c, c, c, c, c  },
        {	c, c, c, c, c, c, c, c, c, c, c, c  },
        };
        break;
    case 2:
        mergeM = {
        {	b, c, c, c, c, c, c, c, c, c, b, b  },
        {	c, c, c, c, c, c, c, c, c, c, c, b  },
        {	c, b, b, b, b, b, b, b, b, c, c, c  },
        {	b, b, b, b, b, b, b, b, b, c, c, c  },
        {	b, b, b, b, b, b, b, b, b, c, c, c  },
        {	b, c, c, c, c, c, c, c, c, c, c, b  },
        {	b, c, c, c, c, c, c, c, c, c, b, b  },
        {	c, c, c, b, b, b, b, b, b, b, b, b  },
        {	c, c, c, b, b, b, b, b, b, b, b, b  },
        {	c, c, c, b, b, b, b, b, b, b, b, b  },
        {	c, c, c, c, c, c, c, c, c, c, c, c  },
        {	c, c, c, c, c, c, c, c, c, c, c, c  },
        };
        break;
    case 3:
        mergeM = {
        {	b, c, c, c, c, c, c, c, c, c, b, b  },
        {	c, c, c, c, c, c, c, c, c, c, c, b  },
        {	c, b, b, b, b, b, b, b, c, c, c, c  },
        {	b, b, b, b, b, b, b, b, b, c, c, c  },
        {	b, b, b, b, b, b, b, b, b, c, c, b  },
        {	b, b, c, c, c, c, c, c, c, c, b, b  },
        {	b, b, c, c, c, c, c, c, c, c, b, b  },
        {	b, b, b, b, b, b, b, b, b, c, c, b  },
        {	b, b, b, b, b, b, b, b, b, c, c, c  },
        {	c, b, b, b, b, b, b, b, c, c, c, c  },
        {	c, c, c, c, c, c, c, c, c, c, c, b  },
        {	b, c, c, c, c, c, c, c, c, c, b, b  },
        };
        break;
    case 4:
        mergeM = {
        {	c, c, c, b, b, b, b, b, b, c, c, c  },
        {	c, c, c, b, b, b, b, b, b, c, c, c  },
        {	c, c, c, b, b, b, b, b, b, c, c, c  },
        {	c, c, c, b, b, b, b, b, b, c, c, c  },
        {	c, c, c, b, b, b, b, b, b, c, c, c  },
        {	c, c, c, c, c, c, c, c, c, c, c, c  },
        {	c, c, c, c, c, c, c, c, c, c, c, c  },
        {	b, b, b, b, b, b, b, b, b, c, c, c  },
        {	b, b, b, b, b, b, b, b, b, c, c, c  },
        {	b, b, b, b, b, b, b, b, b, c, c, c  },
        {	b, b, b, b, b, b, b, b, b, c, c, c  },
        {	b, b, b, b, b, b, b, b, b, c, c, c  },
        };
        break;
    case 5:
        mergeM = {
        {	c, c, c, c, c, c, c, c, c, c, c, c  },
        {	c, c, c, c, c, c, c, c, c, c, c, c  },
        {	c, c, c, b, b, b, b, b, b, b, b, b  },
        {	c, c, c, b, b, b, b, b, b, b, b, b  },
        {	c, c, c, b, b, b, b, b, b, b, b, b  },
        {	c, c, c, c, c, c, c, c, c, c, b, b  },
        {	c, c, c, c, c, c, c, c, c, c, c, b  },
        {	b, b, b, b, b, b, b, b, b, c, c, c  },
        {	b, b, b, b, b, b, b, b, b, c, c, c  },
        {	c, b, b, b, b, b, b, b, b, c, c, c  },
        {	c, c, c, c, c, c, c, c, c, c, c, b  },
        {	b, c, c, c, c, c, c, c, c, c, b, b  },
        };
        break;
    case 6:
        mergeM = {
        {	b, b, c, c, c, c, c, c, c, c, c, b  },
        {	b, c, c, c, c, c, c, c, c, c, c, b  },
        {	c, c, c, b, b, b, b, b, b, b, b, b  },
        {	c, c, b, b, b, b, b, b, b, b, b, b  },
        {	c, c, b, b, b, b, b, b, b, b, b, b  },
        {	c, c, b, b, c, c, c, c, c, c, b, b  },
        {	c, c, c, c, c, c, c, c, c, c, c, b  },
        {	c, c, c, b, b, b, b, b, b, c, c, c  },
        {	c, c, c, b, b, b, b, b, b, c, c, c  },
        {	c, c, c, b, b, b, b, b, b, c, c, c  },
        {	b, c, c, c, c, c, c, c, c, c, c, b  },
        {	b, b, c, c, c, c, c, c, c, c, b, b  },
        };
        break;
    case 7:
        mergeM = {
        {	c, c, c, c, c, c, c, c, c, c, c, c  },
        {	c, c, c, c, c, c, c, c, c, c, c, c  },
        {	c, c, c, b, b, b, b, b, b, c, c, c  },
        {	c, c, c, b, b, b, b, b, c, c, c, b  },
        {	b, b, b, b, b, b, b, c, c, c, b, b  },
        {	b, b, b, b, b, b, b, c, c, c, b, b  },
        {	b, b, b, b, b, b, c, c, c, b, b, b  },
        {	b, b, b, b, b, b, c, c, c, b, b, b  },
        {	b, b, b, b, b, c, c, c, b, b, b, b  },
        {	b, b, b, b, b, c, c, c, b, b, b, b  },
        {	b, b, b, b, c, c, c, b, b, b, b, b  },
        {	b, b, b, b, c, c, c, b, b, b, b, b  },
        };
        break;
    case 8:
        mergeM = {
        {	b, b, b, c, c, c, c, c, c, b, b, b  },
        {	b, c, c, c, c, c, c, c, c, c, c, b  },
        {	b, c, c, b, b, b, b, b, b, c, c, b  },
        {	c, c, b, b, b, b, b, b, b, b, c, c  },
        {	b, c, c, b, b, b, b, b, b, c, c, b  },
        {	b, b, c, c, c, c, c, c, c, c, b, b  },
        {	b, b, c, c, c, c, c, c, c, c, b, b  },
        {	b, c, c, b, b, b, b, b, b, c, c, b  },
        {	c, c, b, b, b, b, b, b, b, b, c, c  },
        {	b, c, c, b, b, b, b, b, b, c, c, b  },
        {	b, c, c, c, c, c, c, c, c, c, c, b  },
        {	b, b, b, c, c, c, c, c, c, b, b, b  },
        };
        break;
    case 9:
        mergeM = {
        {	b, b, c, c, c, c, c, c, c, c, b, b  },
        {	b, c, c, c, c, c, c, c, c, c, c, b  },
        {	c, c, c, b, b, b, b, b, b, c, c, c  },
        {	c, c, b, b, b, b, b, b, b, b, c, c  },
        {	c, c, c, b, b, b, b, b, b, c, c, c  },
        {	b, c, c, c, c, c, c, c, c, c, c, c  },
        {	b, b, c, c, c, c, c, c, c, c, c, c  },
        {	b, b, b, b, b, b, b, b, b, c, c, c  },
        {	b, b, b, b, b, b, b, b, b, c, c, c  },
        {	c, c, b, b, b, b, b, b, c, c, c, c  },
        {	b, c, c, c, c, c, c, c, c, c, c, b  },
        {	b, c, c, c, c, c, c, c, c, c, c, b  },
        };
        break;
    default:
        break;
    }

    std::reverse(std::begin(mergeM), std::end(mergeM));
    int x = x0, y = y0;
    for (auto& row : mergeM) {
        for (auto& col : row) {
           set(x, y, col);
            ++x;
        }
        x = x0;
        ++y;
    }
}
