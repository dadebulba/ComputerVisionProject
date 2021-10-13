/*+++++++++++++++++++++++++
  +mmLib 
  +
  +mmHomography.cpp
  +
  +Authors: Alfredo Armanini (armanini@disi.unitn.it)
  +
  +Copyright 2010 mmLab - Disi University of Trento
  +				    http://disi.unitn.it/
  +++++++++++++++++++++++++*/


#include "../mmLib.h"

namespace mmLib {
namespace mmUtilities {
mmHomography::mmHomography()
{

}

void mmHomography::createHomographyMatrix(vector<pair<double, double> > pixels, vector<pair<double,double> > metrics, int cameraId, std::string cameraName, bool saveToXmlFile)
{
	_pixels = pixels;
	_metrics = metrics;
	_cameraId = cameraId;
	_cameraName = cameraName;
	_error = false;
	_computeHomography(saveToXmlFile);
}
void mmHomography::createHomographyMatrix(std::string xmlInputFile,int cameraId, std::string cameraName, bool saveToXmlFile)
{
		_cameraId = cameraId;
		_cameraName = cameraName;
		ifstream ifile(xmlInputFile.c_str());
		if(ifile)
		{
			TiXmlDocument doc(xmlInputFile.c_str());
			doc.LoadFile();
			TiXmlElement* rootElement = doc.RootElement();

			std::string xmlRootName(rootElement->Value());
			if(xmlRootName=="HomographyInput")
			{
				TiXmlNode* nodeChild;
				for ( nodeChild = rootElement->FirstChild(); nodeChild != 0; nodeChild = nodeChild->NextSibling())
				{
					std::string childName(nodeChild->Value());
					std::cerr << nodeChild->Value() << "\n";
					if(childName=="Point")
					{
						TiXmlNode* nodePointChild;
						for ( nodePointChild = nodeChild->FirstChild(); nodePointChild != 0; nodePointChild = nodePointChild->NextSibling())
						{
							std::cerr << "\t" << nodePointChild->Value() << "\n";
							std::string childPointName(nodePointChild->Value());
							if(childPointName=="Pixel" || childPointName=="Metric")
							{

								int t = nodePointChild->Type();
								if(t==TiXmlNode::TINYXML_ELEMENT)
								{
									TiXmlElement* pElement = nodePointChild->ToElement();
									TiXmlAttribute* pAttrib = pElement->FirstAttribute();
									int attributeNumber=0;
									double xValue,yValue;
									while (pAttrib)
									{
										std::string attributeName(pAttrib->Name());
										if(attributeName=="x" || attributeName=="y")
										{
											std::cerr << "\t\t" << attributeName << "=" << pAttrib->Value() << "\n";
											//Fill vector
											if(attributeName=="x")
												pAttrib->QueryDoubleValue(&xValue);
											else
												pAttrib->QueryDoubleValue(&yValue);
											if(attributeNumber==1)
											{
												if(childPointName=="Pixel" )
													_pixels.push_back(std::pair<double,double>(xValue,yValue));
												else
													_metrics.push_back(std::pair<double,double>(xValue,yValue));
											}
										}
										attributeNumber++;
										pAttrib=pAttrib->Next();
									}
								}
							}
						}
					}
				}
				//		for(int z=0; z<_pixels.size();z++)
				//		{
				//			cout << "Pixel: " << _pixels[z].first << "," << _pixels[z].second << "\n";
				//			cout << "Metric: " << _metrics[z].first << "," << _metrics[z].second << "\n";
				//		}
			}
			_computeHomography(saveToXmlFile);
		}
		else
			_error = true;
}



void mmHomography::loadMatrix(std::string xmlInputFile)
{
	cv::FileStorage openHomo = cv::FileStorage(xmlInputFile.c_str(),cv::FileStorage::READ);
	openHomo["directMatrix"] >> TransfMatrix;
	openHomo["inverseMatrix"] >> InverseTransfMatrix;
	openHomo["id"] >> _cameraId;
	_error = false;
//	float directMatrix[] = {-0.531487, 1.293858, 93.799667, -0.900896, 0.496881, 353.194338, -0.000758, 0.000988, 0.497436};
//	cv::Mat directMat = cv::Mat(3,3,CV_32F,directMatrix).clone();
//	float inverseMatrix[] = {-0.425670, -2.301197, 1714.185535, 0.753599, -0.807349, 431.138388, -0.002146, -0.001903, 3.765854};
//	cv::Mat inverseMat = cv::Mat(3,3,CV_32F,inverseMatrix).clone();
//	cv::FileStorage matXmlOut("text.xml",cv::FileStorage::WRITE);
//	matXmlOut << "id" << 0;
//	matXmlOut << "realtime1" << 0;
//	matXmlOut << "directMatrix" << directMat;
//	matXmlOut << "inverseMatrix" << inverseMat;
}
cv::Point3f mmHomography::PointToMetrics(cv::Point3f pointS)
{
	if(!_error)
	{
		float point[]={pointS.x,pointS.y,pointS.z};
		cv::Mat pixelMat = cv::Mat(3,1,CV_32F,point).clone();
		cv::Mat result = TransfMatrix*pixelMat;
		float x = result.at<float>(0,0);
		float y = result.at<float>(0,1);
		float z = result.at<float>(0,2);
		x/=z;
		y/=z;
		return cv::Point3f(x,y,1);
	}
	else
		return cv::Point3f(0,0,0);
}
cv::Point3f mmHomography::MetricsToPoint(cv::Point3f pointS)
{
	if(!_error)
	{
		float point[] = {pointS.x,pointS.y,pointS.z};
		cv::Mat pixelMat = cv::Mat(3,1,CV_32F,point).clone();
		cv::Mat result = InverseTransfMatrix*pixelMat;
		float x = result.at<float>(0,0);
		float y = result.at<float>(0,1);
		float z = result.at<float>(0,2);
		x/=z;
		y/=z;
		return cv::Point3f(x,y,1);
	}
	else
		return cv::Point3f(0,0,0);
}
int mmHomography::_computeHomography(bool saveToXmlFile){

	//Array creation;
	if(!_error)
	{
		int nPointsPixel = _pixels.size();
		int nPointsMetric = _metrics.size();
		if(nPointsPixel!=nPointsMetric)
			return -1;
		if(nPointsPixel<4)
			return -1;

		double pixelArray[nPointsPixel][2];
		double metricArray[nPointsPixel][2];

		//Fill array
		for(int i=0;i<nPointsPixel;i++)
		{
			pixelArray[i][0]=_pixels[i].first;
			pixelArray[i][1]=_pixels[i].second;
			metricArray[i][0]=_metrics[i].first;
			metricArray[i][1]=_metrics[i].second;
		}
		//		double H[3][3];
		//		double invH[3][3];
		homography(nPointsPixel,pixelArray,metricArray,_H);
		inv3x3(_H, _invH);
		if(saveToXmlFile)
		{
			std::stringstream out;
			out << _cameraId;
			std::string fName = _cameraName+"."+out.str()+".xml";
//			float directMatrix[] = {-0.531487, 1.293858, 93.799667, -0.900896, 0.496881, 353.194338, -0.000758, 0.000988, 0.497436};
			TransfMatrix = cv::Mat(3,3,CV_32F,_H).clone();
//			float inverseMatrix[] = {-0.425670, -2.301197, 1714.185535, 0.753599, -0.807349, 431.138388, -0.002146, -0.001903, 3.765854};
			InverseTransfMatrix = cv::Mat(3,3,CV_32F,_invH).clone();
			cv::FileStorage matXmlOut("text.xml",cv::FileStorage::WRITE);
			matXmlOut << "id" << _cameraId;
			matXmlOut << "realtime" << 0;
			matXmlOut << "directMatrix" << TransfMatrix;
			matXmlOut << "inverseMatrix" << InverseTransfMatrix;
		/*	FILE *fXml;
			std::stringstream out;
			out << _cameraId;
			std::string fName = _cameraName+"."+out.str()+".xml";
			fXml=fopen(fName.c_str(), "w" );
			fprintf( fXml, "<CameraCalibration>\n" );
			fprintf( fXml, "\t<id value=\"%d\" />\n", _cameraId );
			fprintf( fXml, "\t<realTime value=\"0\" />\n" );
			fprintf( fXml, "\t<TransfMatrixDirectRow1 value=\"%f %f %f\" />\n", H[0][0],  H[0][1],  H[0][2]);
			fprintf( fXml, "\t<TransfMatrixDirectRow2 value=\"%f %f %f\" />\n", H[1][0],  H[1][1],  H[1][2]);
			fprintf( fXml, "\t<TransfMatrixDirectRow3 value=\"%f %f %f\" />\n", H[2][0],  H[2][1],  H[2][2]);
			fprintf( fXml, "\t<TransfMatrixInverseRow1 value=\"%f %f %f\" />\n", invH[0][0],  invH[0][1],  invH[0][2]);
			fprintf( fXml, "\t<TransfMatrixInverseRow2 value=\"%f %f %f\" />\n", invH[1][0],  invH[1][1],  invH[1][2]);
			fprintf( fXml, "\t<TransfMatrixInverseRow3 value=\"%f %f %f\" />\n", invH[2][0],  invH[2][1],  invH[2][2]);
			fprintf( fXml, "</CameraCalibration>\n" );
			fclose( fXml );
			*/
		}
		return 1;
	}
	else
		return -1;
}
mmHomography::~mmHomography() {
}
}
}
