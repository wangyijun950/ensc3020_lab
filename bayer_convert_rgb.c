/////////////////////////////////////////////////////////////
// ConvertBayer8ToBgr()
// Converts raw BayerGR8 pixels into
//     BGR pixels.
//
//  G | R | G | R              B G R | B G R | B G R | B G R
// --- --- --- ---            ------- ------- ------- -------
//  B | G | B | G        |\    B G R | B G R | B G R | B G R
// --- --- --- ---  -----  \  ------- ------- ------- -------
//  G | R | G | R   -----  /   B G R | B G R | B G R | B G R
// --- --- --- ---       |/   ------- ------- ------- -------
//  B | G | B | G              B G R | B G R | B G R | B G R
//
/////////////////////////////////////////////////////////////
void ConvertBayer8ToBGR(VmbUchar_t* bayerImgDat, VmbUchar_t* bgrOutputDat)
{
    VmbUchar_t* newimagedata_start = bgrOutputDat;

    int currentTempIndex = 0;
    int nearestBluesAvg = 0;
    int nearestRedsAvg = 0;
    int nearestGreensAvg = 0;

    for(int j = 0; j < 1100; j++)
    {
        for(int i = 0; i < 2752; i++) //G R G R G... 
        {
            if(currentTempIndex % 2 == 0 /* even, green */)
            {
                //avg blue
                if(j == 0) //if in the first row, only take next blue
                {
                    nearestBluesAvg = *(bayerImgDat+currentTempIndex+2752);
                }
                else
                {
                    nearestBluesAvg = (*(bayerImgDat + currentTempIndex + 2752) + *(bayerImgDat+currentTempIndex-2752)) / 2;
                }
                *bgrOutputDat = nearestBluesAvg; //b
                bgrOutputDat++;
                *bgrOutputDat = *(bayerImgDat + currentTempIndex); //g
                bgrOutputDat++;
                //avg red
                if(i == 0) //if in first column, only take next red 
                {
                    nearestRedsAvg = *(bayerImgDat+currentTempIndex+1);
                }
                else
                {
                    nearestRedsAvg = ( (*(bayerImgDat+currentTempIndex+1)) + (*(bayerImgDat+currentTempIndex-1)) ) / 2;
                }
                *bgrOutputDat = nearestRedsAvg; //r
                bgrOutputDat++;

                currentTempIndex++;
            }
            else /* odd, red*/
            {
                //avg blue
                if(i == 1099) //if in last column, take just left-down blue pixel
                {
                    nearestBluesAvg = *(bayerImgDat+currentTempIndex-1+2752);
                }
                else // else take both left-down and right-down
                {
                    nearestBluesAvg = (*(bayerImgDat+currentTempIndex+1+2752) + *(bayerImgDat+currentTempIndex-1+2752)) / 2;
                }
                *bgrOutputDat = nearestBluesAvg; //b
                bgrOutputDat++;
                //avg green
                nearestGreensAvg = (*(bayerImgDat+currentTempIndex-1) + *(bayerImgDat+currentTempIndex+2752)) / 2;
                *bgrOutputDat = nearestGreensAvg;  //g
                bgrOutputDat++;
                *bgrOutputDat = *(bayerImgDat + currentTempIndex); //r
                bgrOutputDat++;

                currentTempIndex++;
            }
        }
        for(int i = 0; i < 2752; i++)//B G B G B G B....
        {
            if(currentTempIndex % 2 == 0 /* even, blue */)
            {

                *bgrOutputDat = *(bayerImgDat + currentTempIndex); //b
                bgrOutputDat++;
                //avg green
                nearestGreensAvg = (*(bayerImgDat + currentTempIndex + 1) + *(bayerImgDat + currentTempIndex -2752)) / 2;
                *bgrOutputDat = nearestGreensAvg; //g
                bgrOutputDat++;
                //avg red
                if(i == 0) //if first column, take only right-up pixel
                {
                    nearestRedsAvg = *(bayerImgDat+currentTempIndex+1-2752);
                }
                else //else take both left-up and right-up pixels
                {
                    nearestRedsAvg = (*(bayerImgDat+currentTempIndex-1-2752) + *(bayerImgDat+currentTempIndex+1-2752)) / 2;
                }
                *bgrOutputDat = nearestRedsAvg; //r
                bgrOutputDat++;

                currentTempIndex++;

            }
            else /* odd, green*/
            {
                //avg blue
                if(i == 2751) //if in last column, only take previous blue (next blue doesnt exist)
                {
                    nearestBluesAvg = *(bayerImgDat + currentTempIndex - 1);
                }
                else //else take both next and previous
                {
                    nearestBluesAvg = (*(bayerImgDat+currentTempIndex+1) + *(bayerImgDat+currentTempIndex-1)) / 2;
                }
                *bgrOutputDat = nearestBluesAvg; //b
                bgrOutputDat++;
                *bgrOutputDat = *(bayerImgDat + currentTempIndex); //g
                bgrOutputDat++;
                //avg red
                if(j == 1099) //if in last row, only take previous red (next red doesn't exist)
                {
                    nearestRedsAvg = *(bayerImgDat+currentTempIndex-2752);
                }
                else //else take both
                {
                    nearestRedsAvg = (*(bayerImgDat+currentTempIndex+2752) + *(bayerImgDat+currentTempIndex-2752)) / 2;
                }
                *bgrOutputDat = nearestRedsAvg; //r
                bgrOutputDat++;

                currentTempIndex++;
            }
        }
    }


    bgrOutputDat = newimagedata_start;
