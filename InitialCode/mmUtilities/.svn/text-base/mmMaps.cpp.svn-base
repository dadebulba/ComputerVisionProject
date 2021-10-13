/*+++++++++++++++++++++++++
  +mmLib 
  +
  +mmMaps.cpp
  +
  +Authors: Alfredo Armanini (armanini@disi.unitn.it)
  +
  +Copyright 2010 mmLab - Disi University of Trento
  +				    http://disi.unitn.it/
  +++++++++++++++++++++++++*/
#include "../mmLib.h"

namespace mmLib {

namespace mmUtilities{

mmMaps::mmMaps() {
	shift=20;
}
cv::Mat mmMaps::buildMap(std::string mapXmlFile)
{
	_error = false;
//	std::vector<cv::Point> points;
//	points.push_back(cv::Point(240,0));
//	points.push_back(cv::Point(420,0));
//	points.push_back(cv::Point(420,65));
//	points.push_back(cv::Point(300,65));
//	points.push_back(cv::Point(300,278));
//	points.push_back(cv::Point(240,278));
	ifstream ifile(mapXmlFile.c_str());
	cv::Mat mapImage;
	int offset=shift;
	if(ifile)
	{
		TiXmlDocument doc(mapXmlFile.c_str());
		doc.LoadFile();
		TiXmlElement* rootElement = doc.RootElement();
		std::string xmlRootName(rootElement->Value());
		if(xmlRootName=="Map")
		{
			TiXmlAttribute* pAttrib = rootElement->FirstAttribute();
			int widthValue,heightValue;
			while (pAttrib)
			{
				std::string attributeName(pAttrib->Name());
				if(attributeName=="width" || attributeName=="height")
				{
					std::cerr << "\t\t" << attributeName << "=" << pAttrib->Value() << "\n";
					//Fill vector
					if(attributeName=="width")
						pAttrib->QueryIntValue(&widthValue);
					else
						pAttrib->QueryIntValue(&heightValue);
				}
				pAttrib=pAttrib->Next();
			}
			mapImage = cv::Mat(cv::Size(widthValue+(2*offset),heightValue+(2*offset)),CV_8UC1,cv::Scalar(255));

			//cv::imwrite("out.jpg",mapImage);
			TiXmlNode* nodeChild;
			for ( nodeChild = rootElement->FirstChild(); nodeChild != 0; nodeChild = nodeChild->NextSibling())
			{
				std::string childName(nodeChild->Value());
				std::cerr << nodeChild->Value() << "\n";
				if(childName=="Object")
				{
					std::vector<cv::Point> points;
					TiXmlNode* nodePointChild;
					for ( nodePointChild = nodeChild->FirstChild(); nodePointChild != 0; nodePointChild = nodePointChild->NextSibling())
					{
						std::cerr << "\t" << nodePointChild->Value() << "\n";
						std::string childPointName(nodePointChild->Value());
						if(childPointName=="Point")
						{

							int t = nodePointChild->Type();
							if(t==TiXmlNode::TINYXML_ELEMENT)
							{
								TiXmlElement* pElement = nodePointChild->ToElement();
								TiXmlAttribute* pAttrib = pElement->FirstAttribute();
								int attributeNumber=0;
								int xValue,yValue;
								while (pAttrib)
								{
									std::string attributeName(pAttrib->Name());
									if(attributeName=="x" || attributeName=="y")
									{
										std::cerr << "\t\t" << attributeName << "=" << pAttrib->Value() << "\n";
										//Fill vector
										if(attributeName=="x")
											pAttrib->QueryIntValue(&xValue);
										else
											pAttrib->QueryIntValue(&yValue);
									}
									pAttrib=pAttrib->Next();
								}
								points.push_back(cv::Point(xValue+offset+1,yValue+offset+1));
							}
						}
					}
					cv::fillConvexPoly(mapImage,&points[0],points.size(),cv::Scalar(200));

				}
				if(childName=="Line")
				{
					int t = nodeChild->Type();
					if(t==TiXmlNode::TINYXML_ELEMENT)
					{
						TiXmlElement* pElement = nodeChild->ToElement();
						TiXmlAttribute* pAttrib = pElement->FirstAttribute();
						int attributeNumber=0;
						int startX,startY,endX,endY;
						while (pAttrib)
						{
							std::string attributeName(pAttrib->Name());
							if(attributeName=="startX" || attributeName=="startY" || attributeName=="endX" || attributeName=="endY")
							{
								std::cerr << "\t\t" << attributeName << "=" << pAttrib->Value() << "\n";
								//Fill vector
								if(attributeName=="startX")
									pAttrib->QueryIntValue(&startX);
								if(attributeName=="startY")
									pAttrib->QueryIntValue(&startY);
								if(attributeName=="endX")
									pAttrib->QueryIntValue(&endX);
								if(attributeName=="endY")
									pAttrib->QueryIntValue(&endY);
							}
							pAttrib=pAttrib->Next();
						}
						cv::line(mapImage,cv::Point(startX+offset,startY+offset),cv::Point(endX+offset,endY+offset),cv::Scalar(0),2);
					}

				}
			}
			//		for(int z=0; z<_pixels.size();z++)
			//		{
			//			cout << "Pixel: " << _pixels[z].first << "," << _pixels[z].second << "\n";
			//			cout << "Metric: " << _metrics[z].first << "," << _metrics[z].second << "\n";
			//		}
		}
	}
	else
		_error = true;
	//cv::imwrite("out.jpg",mapImage);
	cv::Mat coloredImg;
	cv::cvtColor(mapImage,coloredImg,CV_GRAY2RGB);
	return coloredImg;
}
mmMaps::~mmMaps() {
}
}
}
