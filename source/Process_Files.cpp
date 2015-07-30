/*
 * Process_Files.cpp
 *
 *  Created on: 22 July 2015
 *      Author: DetlevCM
 */

#include<Headers.h>


// Function that handles the input and output files

void ProcessFile(vector< OptPoints >& GaussianData ,ifstream& DataInputFile)
{

	string line;
	OptPoints SingleOptPoint;
	vector< AtomDefinition > SingleMolecule;


	while(DataInputFile.good())
	{

		getline(DataInputFile,line);

		//size_t found = line.find("Input orientation:");
		size_t found = line.find("Standard orientation:");

		if (found!=string::npos) // We have found a Z-Matrix
		{

			/*                           Input orientation:
			 * ---------------------------------------------------------------------
			 * Center     Atomic      Atomic             Coordinates (Angstroms)
			 * Number     Number       Type             X           Y           Z
			 * ---------------------------------------------------------------------
			 *       1          6           0       -1.428283    1.396103    0.000000
			 *       2          1           0       -1.962906    1.016650    0.878242
			 * ...
			 *      19          1           0        0.540584    1.221124   -0.889066
			 *      20          1           0        0.540584    1.221124    0.889066
			 *  ---------------------------------------------------------------------
			 */

			getline(DataInputFile,line);
			getline(DataInputFile,line);
			getline(DataInputFile,line);
			getline(DataInputFile,line);
			// pass the "gibberish" (header & table setup)


			getline(DataInputFile,line); // this is our first data setup




			SingleMolecule.clear();

			do{
				// now work on line
				AtomDefinition AtomsInMolecule;

				char * cstr, *p;
				string str = line;
				vector< double > temp;

				cstr = new char [str.size()+1];
				strcpy (cstr, str.c_str());

				p=strtok (cstr," 	");
				while (p!=NULL)
				{
					temp.push_back(strtod(p,NULL));
					p=strtok(NULL," 	");
				}
				delete[] cstr;
				delete[] p;

				// Now sort the vector temp...
				AtomsInMolecule.CenterID = (int) temp[0];
				AtomsInMolecule.AtomNumber = (int) temp[1];
				AtomsInMolecule.AtomType = (int) temp[2];
				AtomsInMolecule.X = temp[3];
				AtomsInMolecule.Y = temp[4];
				AtomsInMolecule.Z = temp[5];

				SingleMolecule.push_back(AtomsInMolecule);

				getline(DataInputFile,line); // this is the following line
			}while(line.find("---")==string::npos); // until lower table line
		}



		// Needs to find after each optimisation
		/*
		 * -----------
		 * For a scan:
		 * -----------
		 * Step number   1 out of a maximum of  111 on scan point     1 out of    26
		 *
		 *------------------------
		 * For an IRC calculation:
		 * -----------------------
		 * # OF POINTS ALONG THE PATH =  64
		 * # OF STEPS =   1
		 *
		 * Calculating another point on the path.
		 * Point Number 65 in FORWARD path direction.
		 * Using LQA Reaction Path Following.
		 *
		 *----------------------------
		 * For a regular Optimisation:
		 * ---------------------------
		 * Berny optimization.
		 * Internal  Forces:  Max     0.000000152 RMS     0.000000028
		 * Search for a saddle point.
		 * Step number  17 out of a maximum of  155
		 *
		 *
		 */

		/*
		 * Could potentially add a check for final geometry:
		 *
		 * --------
		 * Success:
		 * --------
		 *
		 * Optimization completed.
		 * -- Stationary point found.
		 *                           ----------------------------
		 *                           !   Optimized Parameters   !
		 *                           ! (Angstroms and Degrees)  !
		 * --------------------------                            --------------------------
		 * ! Name  Definition              Value          Derivative Info.                !
		 * --------------------------------------------------------------------------------
		 * ! R1    R(1,2)                  1.3583         -DE/DX =    0.0                 !
		 * ..
		 *
		 *
		 * -------
		 * Failed:
		 * -------
		 *
		 * Optimization stopped.
		 * -- Number of steps exceeded,  NStep= 111
		 * -- Flag reset to prevent archiving.
		 * 	                       ----------------------------
		 * 	                       ! Non-Optimized Parameters !
		 * 	                       ! (Angstroms and Degrees)  !
		 * --------------------------                            --------------------------
		 * ! Name  Definition              Value          Derivative Info.                !
		 * --------------------------------------------------------------------------------
		 * ! R1    R(1,2)                  1.0945         -DE/DX =   -0.0002              !
		 * ...
		 */


		found = line.find("Step number");
		if (found!=string::npos) // We have found Step Identification
		{
			char * cstr, *p;
			string str = line;
			vector< string > temp;

			cstr = new char [str.size()+1];
			strcpy (cstr, str.c_str());

			p=strtok (cstr," 	");
			while (p!=NULL)
			{
				temp.push_back(p);
				//cout << p << "\n";
				p=strtok(NULL," 	");
			}
			delete[] cstr;
			delete[] p;


			// Now pick the points we need
			// 2, 8, 12, 14

			SingleOptPoint.ScanStep = strtod(temp[2].c_str(),NULL);
			SingleOptPoint.OptStep = strtod(temp[8].c_str(),NULL);
			SingleOptPoint.OptStepMax = strtod(temp[12].c_str(),NULL);
			SingleOptPoint.Molecule = SingleMolecule;

			GaussianData.push_back(SingleOptPoint);
			SingleMolecule.clear();
		}



	}


	if((int) SingleMolecule.size() > 0)
	{
		SingleOptPoint.ScanStep = 0;
		SingleOptPoint.OptStep = 0;
		SingleOptPoint.OptStepMax = 0;
		SingleOptPoint.Molecule = SingleMolecule;

		GaussianData.push_back(SingleOptPoint);
		SingleMolecule.clear();
	}

}
