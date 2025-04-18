#include<kalman/PredictorKalman.h>

kal_test::kal_test(){
    _Kalman::Matrix_xxd A = _Kalman::Matrix_xxd::Identity(); //单位矩阵
    _Kalman::Matrix_zxd H{1,0};
    _Kalman::Matrix_xxd R;
    R(0, 0) = 0.01;
    for (int i = 1; i < S; i++) {
        R(i, i) = 100;
    }
    _Kalman::Matrix_zzd Q{4};
    _Kalman::Matrix_x1d init{0, 0};
    kalman = _Kalman(A, H, R, Q, init, 0);
}

void kal_test::reset(double x,double t) { kalman.reset(x,t); }

double kal_test::kal_run(double x,double now_t,double dt){
    _Kalman::Matrix_z1d temp{x};
    _Kalman::Matrix_x1d temp2;
    temp2=kalman.update(temp,now_t);
    dt+=now_t-last_t;
    last_t=now_t;
    double ans=temp2(0,0)+dt*temp2(1,0);
    return ans;
}