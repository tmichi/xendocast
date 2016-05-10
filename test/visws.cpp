#include <mi/VolumeData.hpp>
#include <mi/AttributeSet.hpp>
#include <mi/VolumeDataImporter.hpp>
#include <mi/Mesh.hpp>
#include <mi/MeshExporterPlyWithColor.hpp>
#include <mi/VolumeDataUtility.hpp>
#include <mi/ColorMapper.hpp>
int main ( int argc, char** argv ) {
        std::string df_file;
        std::string label_file;
        std::string out_file;
        double scale;
        int z;
        mi::Point3i size;

        mi::Argument arg(argc, argv);
        mi::AttributeSet attrSet;
        attrSet.createStringAttribute("-d", df_file).setMandatory();
        attrSet.createStringAttribute("-l", label_file).setMandatory();
        attrSet.createStringAttribute("-o", out_file).setMandatory();
        attrSet.createNumericAttribute<double>("-s", scale).setDefaultValue(5);
        attrSet.createTripleNumericAttribute<int>("-size", size.x(), size.y(), size.z()).setMandatory().setMin(1,1,1);
        attrSet.createNumericAttribute<int>("-z", z).setMin(1).setDefaultValue(150);

        if ( attrSet.parse(arg) == false ) return -1;

        mi::VolumeInfo info ( size);
        mi::VolumeData<char> labelData(info);
        mi::VolumeData<float> distData(info);

        mi::VolumeDataUtility::open(distData, df_file);
        mi::VolumeDataUtility::open(labelData, label_file);

        std::vector<mi::Color3f> cols;
        mi::Mesh mesh;
        for( int y = 0 ; y < size.y()  ; ++y ) {
                for( int x = 0 ; x < size.x() ; ++x ) {
                        mi::Vector3d p;
                        p.x() = x ; 
                        p.y() = y ; 
                        p.z() = -scale * distData.get(x,y,z);

                        mesh.addPoint(p);

                        mi::Color3f c;
                        
                        switch ( labelData.get(x,y,z) ) {
                        case 0:
                              if ( distData.get(x,y,z) > 0 ) c = mi::Color3f(0.5, 0.5, 0.5);
                              else c = mi::Color3f (1, 1, 1);
                               break;
                        case 1 : 
                                c = mi::Color3f( 1, 1, 0);
                                break;
                        case 2 : 
                                c = mi::Color3f(1, 0, 0);
                                break;
                        default:
                                c = mi::Color3f (0,0,0);
                                break;                                
                        }
                    
                        cols.push_back(c);

                }
        }

        for( int y = 0 ; y < size.y() -1 ; ++y ) {
                for( int x = 0 ; x < size.x()-1 ; ++x ) {
                        std::vector<int> idx0, idx1;

                        int v00 = x + y * size.x();
                        int v01 = v00 + 1;
                        int v10 = v00 +  size.x();
                        int v11 = v10 + 1;
                        idx0.push_back(v00);
                        idx0.push_back(v10);
                        idx0.push_back(v01);

                        idx1.push_back(v11);
                        idx1.push_back(v01);
                        idx1.push_back(v10);

                        mesh.addFace(idx0);
                        mesh.addFace(idx1);

                }
        }

        mi::MeshExporterPlyWithColor(mesh,cols).write(out_file);

        return 0;
}