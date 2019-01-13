
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <fstream>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/Object.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include<ctime>
#include <string>
#include<sstream>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>

const int MAX_VALID_YR = 9999; 
const int MIN_VALID_YR = 1800; 
  
// Returns true if  
// given year is valid. 
bool isLeap(int year) 
{ 
// Return true if year  
// is a multiple pf 4 and 
// not multiple of 100. 
// OR year is multiple of 400. 
return (((year % 4 == 0) &&  
         (year % 100 != 0)) || 
         (year % 400 == 0)); 
} 

// Returns true if given 
// year is valid or not. 
bool isValidDate(int d, int m, int y) 
{ 
    // If year, month and day  
    // are not in given range 
    if (y > MAX_VALID_YR ||  
        y < MIN_VALID_YR) 
    return false; 
    if (m < 1 || m > 12) 
    return false; 
    if (d < 1 || d > 31) 
    return false; 
  
    // Handle February month  
    // with leap year 
    if (m == 2) 
    { 
        if (isLeap(y)) 
        return (d <= 29); 
        else
        return (d <= 28); 
    } 
  
    // Months of April, June,  
    // Sept and Nov must have  
    // number of days less than 
    // or equal to 30. 
    if (m == 4 || m == 6 || 
        m == 9 || m == 11) 
        return (d <= 30); 
  
    return true; 
} 
/**
 * List objects (keys) within an Amazon S3 bucket.
 */
std::string timePointAsString(const std::chrono::system_clock::time_point& tp) {
   
    //convert time point to time_t
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
  
    struct tm* str;
  
   //convert time_t to struct tm
    str = gmtime(&t);
  
    char ptr[100] ;


    //convert to array
    strftime(ptr,100,"%Y/%m/%d",str);


   //convert to string
    std::string v(ptr);
 
    return v;
}
 int find_diffdays( struct std::tm* a, struct std::tm* b)
{
    
    std::time_t x = std::mktime(a);
    std::time_t y = std::mktime(b);
    double difference;
    if ( x != (std::time_t)(-1) && y != (std::time_t)(-1) )
    {
        difference = std::difftime(y, x) / (60 * 60 * 24);
   
       return difference;
    }
    return difference;
}

template <typename Clock, typename Duration>
std::ostream& operator<<(std::ostream& os, const std::chrono::time_point<Clock, Duration>& timep)
{
    auto converted_timep = Clock::to_time_t(timep);
    os << std::put_time(std::localtime(&converted_timep), "%Y/%m/%d ");
    return os;
} 

auto convert_to_timepoint(int years, int months, int days)
{
    years -= 1900; //epoch
    std::tm date = {};
    date.tm_year = years;
    date.tm_mon = months;
    date.tm_mday = days;

    return std::chrono::system_clock::from_time_t(std::mktime(&date));
}

template <typename Clock, typename Duration>
auto add_days(const std::chrono::time_point<Clock, Duration>& timepoint, int days_to_add)
{
    constexpr std::time_t seconds_in_day = 60 * 60 * 24;
    //                                     mm   hh   dd

    std::time_t days = seconds_in_day * days_to_add;
    auto date = Clock::to_time_t(timepoint);
    return Clock::from_time_t(date + days);
}




/////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{

std::string station_id;

std::cout << "enter station id"<<std::endl;
std::cin>>station_id;

// get home directory
std::string homedir = getenv("HOME");
std::cout<<homedir<<std::endl;

// create path of station folder
std::string pathname = homedir + "/"+station_id;

std::cout<<pathname<<std::endl;

// check if folder exists, if it doesnt create it
struct stat info;

if( stat( pathname.c_str(), &info ) != -1 )
    std::cout<< pathname << "exists"<<std::endl;
else
{
   int create = mkdir(pathname.c_str(),ACCESSPERMS);
}

std::string startdate;
std::cout << "enter start date in format YYYY/MM/DD"<<std::endl;
std::cin>>startdate;
std::string delimiter = "/"; 

int startday,startmonth,startyear, endday,endmonth,endyear;
auto start = startdate.begin(); // iterator at beginning of string
auto finish = startdate.find(delimiter); // gives position of first occurrence of delimiter
if (finish == startdate.npos) // find returned the end of string
 std::cout<< "Please enter date correctly"<<std::endl;
else
{
  startyear = stoi(startdate.substr(0, finish)); // Extracts year part from date string
  startdate = startdate.substr(finish+1); // Removes the month part from the date string
  //std::cout<<startyear<<std::endl;

}

 start = startdate.begin(); // iterator at beginning of string
 finish = startdate.find(delimiter); // gives position of first occurrence of delimiter
if (finish == startdate.npos) // find returned the end of string
 std::cout<< "Please enter date correctly"<<std::endl;
else
{
   startmonth = stoi(startdate.substr(0, finish)); // Extracts month part from date string
  startdate = startdate.substr(finish+1); // Removes the month part from the date string
  //std::cout<<startmonth<<std::endl;

}
  startday = stoi(startdate); 
 

   if(!isValidDate(startday,startmonth,startyear))
      {
          std::cout<<"Invalid start date"<<std::endl;
          exit(0);
      }

    std::string enddate;
    std::cout<<"Enter end date in format YYYY/MM/DD"<<std::endl;
    std::cin>>enddate;



auto newstart = enddate.begin(); // iterator at beginning of string
auto newfinish = enddate.find(delimiter); // gives position of first occurrence of delimiter
if (newfinish == enddate.npos) // find returned the end of string
 std::cout<< "Please enter date correctly"<<std::endl;
else
{
  endyear = stoi(enddate.substr(0, newfinish)); // Extracts year part from date string
  enddate = enddate.substr(newfinish+1); // Removes the year part from the date string
  //std::cout<<endyear<<std::endl;

}

 newstart = enddate.begin(); // iterator at beginning of string
 newfinish = enddate.find(delimiter); // gives position of first occurrence of delimiter
if (newfinish == enddate.npos) // find returned the end of string
 std::cout<< "Please enter date correctly"<<std::endl;
else
{
    endmonth = stoi(enddate.substr(0, newfinish)); // Extracts month part from date string
  enddate = enddate.substr(newfinish+1); // Removes the month part from the date string
  //std::cout<<endmonth<<std::endl;

}
  endday = stoi(enddate); // Extracts month part from date string
  //std::cout<<endday<<std::endl;
 
    if(!isValidDate(endday,endmonth,endyear))
      {
          std::cout<<"Invalid end date"<<std::endl;
          exit(0);
      }




    // create struct tm pertaining to the start and end dates
    struct std::tm startstruct = {0,0,0,startday,startmonth-1,startyear-1900}; 
    struct std::tm endstruct = {0,0,0,endday,endmonth-1,endyear-1900}; 

    double difference =find_diffdays(&startstruct,&endstruct);

    if(difference < 0)
        {
            std::cout<<"Start date occurs after end date"<<std::endl;
            exit(0);
        }

  //  std::cout << "difference = " << difference << " days" << std::endl;

std::string delim = "/";
  


    auto date = convert_to_timepoint(startyear, startmonth-1, startday);
    std::cout << "Given Day :" << date << '\n';
  

   std::vector<std::string>alldays;
   alldays.push_back(timePointAsString(date)+delim+station_id);
   
   for(int i=0;i<difference;i++)
       
    {  date = add_days(date,1);
       std::cout << "Day :" << date<<std::endl;
      
       alldays.push_back(timePointAsString(date)+delim+station_id);
    

    }
   
   //AWS 
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        std::cout<<"Say"<<alldays[0].c_str()<<std::endl;
        
        if (argc < 2)
        {
            std::cout << std::endl <<
                "To run this example, supply the name of a bucket to list!" <<
                std::endl << "Ex: list_objects <bucket-name>" << std::endl
                << std::endl;
            exit(1);
        }

        const Aws::String bucket_name = argv[1];
        std::cout << "Objects in S3 bucket: " << bucket_name << std::endl;

        //AWS s3 client object
        Aws::S3::S3Client s3_client;



for(int i=0;i<alldays.size();i++)
{
        Aws::S3::Model::ListObjectsRequest listobjects_request;
        listobjects_request.WithBucket(bucket_name).WithPrefix(alldays[i].c_str());
        Aws::S3::Model::ListObjectsOutcome list_objects_outcome; // result comes in this object

      
        size_t totalNumberOfBytes = 0, minFileSize = 0, maxFileSize = 0;
       
        bool done =false;
      while(!done)
      {  
         

        list_objects_outcome = s3_client.ListObjects(listobjects_request); //will list and put the request into the outcomne
         if (list_objects_outcome.IsSuccess())
        
        {
            Aws::Vector<Aws::S3::Model::Object> object_list =
                list_objects_outcome.GetResult().GetContents(); //way to make it iterable

            std::cout << "Number of objects in S3 bucket " << bucket_name << ": " << object_list.size() << std::endl;
            std::string key ("/");
            for (auto const &s3_object : object_list)
            {
                
                Aws::String newstr = s3_object.GetKey();
                std::cout << newstr << std::endl;
                std::string target(newstr.c_str(), newstr.size());
                  std::size_t found = target.rfind(key);
                  std::string finalplace;
                    if (found!=std::string::npos)
                       finalplace = target.substr(found+1);
                  std::cout<<finalplace<<std::endl;                 

                Aws::S3::Model::GetObjectRequest object_request;
                object_request.WithBucket(bucket_name).WithKey(newstr);
               
        Aws::S3::Model::GetObjectOutcome get_object_outcome = s3_client.GetObject(object_request);

        if (get_object_outcome.IsSuccess())
        {
            Aws::OFStream local_file;
            //std::cout<<"Naming"<<finalplace<<std::endl;
            std::string finfile = pathname+"/"+finalplace;
            //local_file.open(newstr.c_str(), std::ios::out | std::ios::binary);
            local_file.open(finfile.c_str(), std::ios::out | std::ios::binary);
            local_file << get_object_outcome.GetResult().GetBody().rdbuf();
            std::cout << "Done!" << std::endl;
        }
        else
        {
            std::cout << "GetObject error: " <<
                get_object_outcome.GetError().GetExceptionName() << " " <<
                get_object_outcome.GetError().GetMessage() << std::endl;
        }
                //" size: " << s3_object.GetSize() << std::endl;
                 totalNumberOfBytes += s3_object.GetSize();
                 if (maxFileSize < s3_object.GetSize())
                    maxFileSize = s3_object.GetSize();
                 if (minFileSize == 0)
                      minFileSize = s3_object.GetSize();
                 else if (minFileSize > s3_object.GetSize())
                    minFileSize = s3_object.GetSize();
            }
            done=!list_objects_outcome.GetResult().GetIsTruncated();
            if(!done) 
            {
                listobjects_request.SetMarker(list_objects_outcome.GetResult().GetContents().back().GetKey());
            }

            
        }
        else
        {
            std::cout << "ListObjects error: " <<
                list_objects_outcome.GetError().GetExceptionName() << " " <<
                list_objects_outcome.GetError().GetMessage() << std::endl;
                break;
        }
    }
            std::cout << "Total number of bytes in bucket  : " << totalNumberOfBytes << std::endl;
            std::cout << "Minimum number of bytes in a file: " << minFileSize << std::endl;
            std::cout << "Maximum number of bytes in a file: " << maxFileSize << std::endl;
    }
    }
    Aws::ShutdownAPI(options);
}