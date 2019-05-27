#include "threshold.h"


using GL = uint8_t;

const GL MAX = numeric_limits<uint8_t>::max();

static vector<double> PG;
static bool PG_CACHED = false;

Mat SAVED;

double p_g_compute(const Mat img, const GL g){
    double res = 0;

    Mat binary_image;
    inRange(img, g, g, binary_image);
    res = countNonZero(binary_image);
    // for(auto it = img.begin<GL>(); it != img.end<GL>(); ++it)
    // {
    //     if(*it == g)
    //         res++;
    // }
    return res/img.total();
}

double p_g(const Mat img, const GL g)
{
    Mat diff = img != SAVED;
    if(PG_CACHED and (countNonZero(diff) == 0))
        return PG.at(g);
    else {
        cout << "Looking for threshold..." << endl;
        for(GL i = 0; i < MAX; ++i)
        {
            PG.push_back(p_g_compute(img, i));
        }
        PG_CACHED = true;
        SAVED = img;
        // cout << "full compute finished" << endl;
    }
    return PG.at(g);
}

double mu0_T(const Mat img, const GL T) {
    double up = 0;
    double bottom = 0;
    for (GL g = 0; g <= T; ++g) {
        up += g * p_g(img, g);
        bottom += p_g(img, g);
    }
    return up/bottom;
}

double mu1_T(const Mat img, const GL T, const GL n) {
    double up = 0;
    double bottom = 0;
    for (GL g = T + 1; g <= n; ++g) {
        up += g * p_g(img, g);
        bottom += p_g(img, g);
    }
    return up/bottom;
}

double E_x(const Mat img, const GL n) {
    double res = 0;
    for(GL g = 0; g <= n; ++g)
    {
        res += g * p_g(img, g);
    }
    return res;
}

double E_y_T(const Mat img, const GL T, const GL n)
{
    double a = 0;
    for(GL g = 0; g <= T; ++g)
    {
        a += mu0_T(img, T) * p_g(img, g);
    }

    double b = 0;
    for(GL g = T+1; g <= n; ++g)
    {
        b += mu1_T(img, T, n) * p_g(img, g);
    }
    return a + b;
}

double E_xy_T(const Mat img, const GL T, const GL n)
{
    double a = 0;
    for(GL g = 0; g <= T; ++g)
    {
        a += g * mu0_T(img, T) * p_g(img, g);
    }

    double b = 0;
    for(GL g = T+1; g <= n; ++g)
    {
        b += g * mu1_T(img, T, n) * p_g(img, g);
    }
    return a + b;
}

double E_xx(const Mat img, const GL n)
{
    double res = 0;
    for(GL g = 0; g <= n; ++g)
    {
        res += g*g * p_g(img, g);
    }
    return res;
}

double E_yy(const Mat img, const GL T, const GL n)
{
    double a = 0;
    for(GL g = 0; g <= T; ++g)
    {
        double mu = mu0_T(img, T);
        double pg = p_g(img, g);
        a += mu*mu * pg;
    }

    double b = 0;
    for(GL g = T+1; g < n; ++g)
    {
        double mu = mu1_T(img, T, n);
        double pg = p_g(img, g);
        b += mu*mu * pg;
    }
    return a + b;
}

double V_x(const Mat img, const GL n)
{
    double e = E_x(img, n);
    return E_xx(img, n) - e*e;
}

double V_y_T(const Mat img, const GL T, const GL n)
{
    double e = E_y_T(img, T, n);
    return E_yy(img, T, n) - e*e;
}

double rho(const Mat img, const GL T, const GL n)
{
    double up = E_xy_T(img, T, n) - E_x(img, n) * E_y_T(img, T, n);
    double pow_arg = V_x(img, n) * V_y_T(img, T, n);
    double bottom = pow(pow_arg, 0.5);
    return up/bottom;
}

int findThreshold(const Mat img)
{

    p_g(img, 0);

    uint best_T = 0;
    double best_rho = -1;
    // singlethreaded
    auto begin = chrono::steady_clock::now();
    for(GL i = 0; i < MAX; ++i)
    {
        double cur_rho = rho(img, i, MAX-1);
//        cout << cur_rho << endl;
//        circle(hist_img, Point(i*width, height - 100*(cur_rho)), 3, Scalar(0, 0, 256), -1);
        // circle(histImage, Point(i*bin_w, hist_h - 100*(cur_rho)), 3, Scalar(0, 0, 256), -1);
        if (cur_rho > best_rho)
        {
            best_rho = cur_rho;
            best_T = i;
        }
    }
    auto end = chrono::steady_clock::now();

    cout << "one in time: " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms" << endl;

    cout << "best rho: " << best_rho << endl;
    cout << "best T: " << best_T << endl;
    return best_T;
}