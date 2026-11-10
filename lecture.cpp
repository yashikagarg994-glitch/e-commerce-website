#include <iostream>
using namespace std;
int main() 
// {
//   char x='A'; int n=3;
//   for(int i=0;i<n;i++){
//     for(int j=0 ; j<n; j++){
//         cout << x << " ";
//         x++;
//     }
//     cout << endl;
//   }
//     int n=4; int x =1 ;
//     for ( int i = 0; i<n ; i++){
//         for( int j = 1 ; j<= i+1; j++){
//             cout << x << " ";
//         }
//         x++;
//         cout << endl;
//     }
    
// }
// {
//     for(int i=0; i<=5 ;i++){
//         for(int j =1 ; j<=i; j++){
//             cout << j ;
//         }
//         cout << endl;
//     }
// }
// {
//     for (int i=1; i<=4 ; i++){
//         for(int j=i ; j>0 ; j--){
//             cout << j;
//         }
//         cout << endl;
//     }
// }
// {
//     for (int i=1 ;i <= 8; i+=2){
//         for(int j=1 ; j<=i ; j++){
//             cout << j;
//         }
//         cout<< endl;
//     }
// }
// {
//     char num = "A"; 
//     for(int i=1 ; i<=4 ; i++){
//         for( int j=1; j<=i ;j++){
//             cout<< num;
//             num++;
//         }
//         cout << endl;
//     }
// }
// {
//     int num=1;
//     for( int i=1 ;i<=4 ; i++){
//         for(int x=1 ; x<=i ; x++){
//             cout<< " ";
//         }
//         for(int j=1 ;j<=i ;j++){
//             cout << num;
//         }
//         num++;
//         cout << endl;
//     }
// }
// {
//     for(int i =0 ; i<4 ; i++){
//         for(int j=3 ; j>i ;j--){
//             cout << "  ";
//         }
//         for(int j=1; j<=i+1 ; j++){
//             cout << j << " ";
//         }
//         for(int j=i; j>=1 ;j--){
//             cout <<j << " ";
//         }
//         cout << endl;
//     }
// }
// {
//     int n =4;
//     for(int i=0 ; i<n ; i++){
//         char x='A';
//         for( int j=0 ; j<n ; j++){
//             cout << x << " ";
//             x+=1;
//         }
//         cout << endl;

//     }
// }
{
    int n=4; 
    for(int i=0 ; i<n ; i++){
        int x=1;
        for(int j=n-i-1 ; j>0 ;j--){
            cout << " ";
        }
        for(int j=0 ; j<=i ; j++){
            cout << x ;
            x++;
        }
        if(i!=0){
            for(int j=i ;j >0 ; j-- ){
                cout << j ;
            }
        }
        cout << endl;
    }
}
