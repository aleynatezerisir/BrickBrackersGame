#define CHMAT_UNSIGNED_CHAR     1
#define CHMAT_UNSIGNED_INT      2
#define CHMAT_INT               3
#define CHMAT_FLOAT             4
#define CHMAT_DOUBLE            5

class CHMAT
{
public:
    //members
    int X = 0, Y = 0, type = 0;
    void* data = 0;
    HBITMAP HBitmap; //bitmap handle
    //---constructer & destructor ---------------------
    CHMAT(int x, int y, int typ);
    ~CHMAT();
    //---methods ---------------------
    template<typename T> int set(int x, int y, T val);
    void print_mat();
    //---operators ------------------
    void operator = (int a);
    template<typename T> void operator += (T a);
    template<typename T> void operator /= (T a);
    void operator *= (CHMAT& a);
    int& operator () (int x, int y);   
    //-------
    void mat_mult(CHMAT& m, CHMAT& a);
    void mat_subst(CHMAT& m, CHMAT& a);
    void mat_add(CHMAT& m, CHMAT& a);
    void mat_equal(CHMAT& m);

    void printnumber(int x0, int y0, int number, int c, int b);

    //

    //-- abdulaziz

    void operator -= (int a);
    void operator -= (double a);
    int getX();
    int getY();
    int getType();
    void* getData();
    void setY(int y);
    void operator += (CHMAT& a);
    //


    //-- ilhan

    void operator *= (int a);
    void operator *= (double a);
    void operator -= (CHMAT& a);
    void mat_subtr(CHMAT& m, CHMAT& a);
    
    //   
};


bool Rectangle(CHMAT& target, int x1, int y1, int width, int height, int color, bool is_filled);
bool Ellipse(CHMAT& target, int x, int y, int width, int height, int col, bool isFill);
void Line_for_ellipse(CHMAT& target, int x1, int y1, int x2, int y2, int color, bool isFill, int o_x, int o_y, int width, int height);
void Line(CHMAT& target, int x1, int y1, int x2, int y2, int color);