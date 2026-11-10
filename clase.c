#include <stdio.h>

int main()
{
    printf("Arshiya Garg\n");
    printf("01611515625\n");
    printf("CSE-AIML P1\n\n");

    int r1, c1, r2, c2;
    int i, j, k;

    printf("Enter rows and columns of Matrix A: ");
    scanf("%d %d", &r1, &c1);

    printf("Enter rows and columns of Matrix B: ");
    scanf("%d %d", &r2, &c2);

    int A[r1][c1], B[r2][c2];

    printf("\nEnter elements of Matrix A:\n");
    for(i = 0; i < r1; i++)
    {
        for(j = 0; j < c1; j++)
        {
            scanf("%d", &A[i][j]);
        }
    }

    printf("\nEnter elements of Matrix B:\n");
    for(i = 0; i < r2; i++)
    {
        for(j = 0; j < c2; j++)
        {
            scanf("%d", &B[i][j]);
        }
    }

    /* Addition and Subtraction */
    if(r1 == r2 && c1 == c2)
    {
        printf("\nMatrix Addition:\n");
        for(i = 0; i < r1; i++)
        {
            for(j = 0; j < c1; j++)
            {
                printf("%d ", A[i][j] + B[i][j]);
            }
            printf("\n");
        }

        printf("\nMatrix Subtraction:\n");
        for(i = 0; i < r1; i++)
        {
            for(j = 0; j < c1; j++)
            {
                printf("%d ", A[i][j] - B[i][j]);
            }
            printf("\n");
        }
    }
    else
    {
        printf("\nAddition and subtraction are not possible.\n");
        printf("Both matrices must have the same dimensions.\n");
    }

    /* Multiplication */
    if(c1 == r2)
    {
        int C[r1][c2];

        for(i = 0; i < r1; i++)
        {
            for(j = 0; j < c2; j++)
            {
                C[i][j] = 0;

                for(k = 0; k < c1; k++)
                {
                    C[i][j] = C[i][j] + A[i][k] * B[k][j];
                }
            }
        }

        printf("\nMatrix Multiplication:\n");
        for(i = 0; i < r1; i++)
        {
            for(j = 0; j < c2; j++)
            {
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }
    }
    else
    {
        printf("\nMultiplication is not possible.\n");
        printf("Columns of Matrix A must be equal to rows of Matrix B.\n");
    }

    return 0;
}