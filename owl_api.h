#ifndef OWL_API.H
#define OWL_API.H

#include "pugixml.hpp"
#include <iostream>
#include<vector>
#include<string>
#include<set>
#include<map>
#include<stack>
#include <stdio.h>
#include<string.h>
#include "parsers.h"


namespace owl_api{

std::set<std::string> GetAllClasses(std::string filename){

    return get_all_something_names("owl:Class", filename);
}


std::set<std::string> GetAllIndividuals (std::string filename){

    return get_all_something_names("owl:NamedIndividual", filename);
}

std::set<std::string> GetAllObjectProperties(std::string filename){

    return get_all_something_names("owl:ObjectProperty", filename);
}

std::set<std::string> GetAllDataProperties(std::string filename){

    return get_all_something_names("owl:DatatypeProperty", filename);
}


std::vector<std::string> get_all_parents(std::map<std::string, std::vector <std::string> > &map_of_names ,
                                         std::string name){
    std::vector<std::string> result;

    std::map<std::string, std::vector <std::string> >::iterator it;
    it = map_of_names.find(name);
    if(it != map_of_names.end()){

        for(int i = 0; i < it->second.size(); ++i){
            result.push_back(it->second[i]);
            std::vector<std::string> result_2 = get_all_parents(map_of_names, it->second[i]);
            if(result_2.size() > 0 ){
                for(int j =0; j < result_2.size(); ++j){
                    result.push_back(result_2[j]);
                }
        }
        }

    }
    std::set<std::string> set_res(result.begin(), result.end());
    std::vector<std::string> final_result(set_res.begin(), set_res.end());
    return final_result;


}



std::vector<std::string> all_children_subprop(
        std::map<std::string, std::vector <std::string> > &sub, std::string name)
        {
            std::vector<std::string> result;
            std::string current_name;
            std::vector<std::string> tmp;

            for(int i =0; i < sub[name].size(); ++i){
                result.push_back(sub[name][i]);
                tmp = all_children_subprop(sub, sub[name][i]);

                for(int j =0; j < tmp.size(); ++j)
                    result.push_back(tmp[j]);
            }


            std::set<std::string> tmp2 = std::set<std::string>(result.begin(), result.end());
            result = std::vector<std::string>(tmp2.begin(), tmp2.end());
            return result;
        }

class loader{

private:
    std::string filename;
    std::set<std::string> AllClasses;
    std::set<std::string> AllIndividuals;
    std::set<std::string> AllObjectProperties;
    std::set<std::string> AllDataProperties;
    std::set<std::string> functional_properties;
    std::set<std::string> inverse_functional_properties;

    std::map<std::string, std::string> inverse;
    std::map<std::string, std::vector <std::string> > class_super_class;
    std::map<std::string, std::vector <std::string> > properties_children;

    std::map<std::string, std::string> IRI_to_filenames;
    std::vector<restriction> restrictions;

    std::set <std::string> connected_restrictions;
    std::set <std::string> connected_restrictions_with_values;
    std::map <std::string, std::vector <std::pair <std::string, std::string> > >
    nameInd_prop_values;

    std::map <std::string, std::vector <std::string> >
    individual_superclasses;

    std::map<std::string, std::vector<std::string> > property_members;
    std::map<std::string, std::vector<std::string> > property_members2;

    std::map <std::string, std::vector <std::string> >
    EquivalentClasses;

    std::vector<std::string> IRIimports;

public:


void map_ontology_IRI(std::string IRI, std::string IRI_filename){
    IRI_to_filenames[IRI] = IRI_filename;
}

void reload(){
    load();
}

loader(){
    std::cout << "created a loader instance" << std::endl;
}

void set_filename(std::string filename_){
    filename = filename_;
}

void add_info(loader& tmp){
    for(std::set<std::string>::iterator it = tmp.AllClasses.begin(); it!= tmp.AllClasses.end(); ++it)
        AllClasses.insert(*it);
    for(std::set<std::string>::iterator it = tmp.AllIndividuals.begin(); it!= tmp.AllIndividuals.end(); ++it)
        AllIndividuals.insert(*it);
    for(std::set<std::string>::iterator it = tmp.AllObjectProperties.begin(); it!= tmp.AllObjectProperties.end(); ++it)
        AllObjectProperties.insert(*it);
    for(std::set<std::string>::iterator it = tmp.AllDataProperties.begin(); it!= tmp.AllDataProperties.end(); ++it)
        AllDataProperties.insert(*it);



    for(std::set<std::string>::iterator it = tmp.functional_properties.begin(); it != tmp.functional_properties.end(); ++it)
        functional_properties.insert(*it);
    for(std::set<std::string>::iterator it = tmp.inverse_functional_properties.begin();
        it!= tmp.inverse_functional_properties.end(); ++it)
        inverse_functional_properties.insert(*it);


    for(std::map<std::string, std::string>::iterator it = tmp.inverse.begin();
        it!=tmp.inverse.end(); ++it)
        inverse[it->first] = it->second;


    for(std::map<std::string, std::vector <std::string> >::iterator it =
        tmp.class_super_class.begin(); it!=tmp.class_super_class.end(); ++it)
        class_super_class[it->first] = it->second;

    for(std::map<std::string, std::vector <std::string> >::iterator it =
        tmp.properties_children.begin(); it!= tmp.properties_children.end(); ++it)
        properties_children[it->first] = it->second;


    for(std::vector<restriction>::iterator it=tmp.restrictions.begin();
        it!=tmp.restrictions.end(); ++it)
        restrictions.push_back(*it);


    /*
    for(std::set <std::string> ::iterator it = tmp.connected_restrictions.begin();
        it!=connected_restrictions.end(); ++it)
        {std::cout << *it << " ";
        //connected_restrictions.insert(*it);
        std::cout <<"Here ";
        }
    */
    for(std::set <std::string>::iterator it=tmp.connected_restrictions_with_values.begin();
        it!=tmp.connected_restrictions_with_values.end(); ++it)
        connected_restrictions_with_values.insert(*it);

    for(std::map <std::string, std::vector <std::pair <std::string, std::string> > >
    ::iterator it = tmp.nameInd_prop_values.begin(); it!= tmp.nameInd_prop_values.end(); ++it)
    nameInd_prop_values[it->first] = it->second;

    for(std::map <std::string, std::vector <std::string> >::iterator it =
    tmp.individual_superclasses.begin(); it!= tmp.individual_superclasses.end(); ++it)
        individual_superclasses[it->first] = it->second;


/**

for(std::map<std::string, std::vector<std::string> >::iterator it = tmp.property_members.begin();
std::map<std::string, std::vector<std::string> >::iterator it tmp.property_members2;

for(std::map <std::string, std::vector <std::string> >::iterator it
EquivalentClasses;

*/


    }

void load(){
    char * char_filename = new char [filename.length()+1];
    strcpy (char_filename, filename.c_str());
    pugi::xml_document doc;
    if (!doc.load_file(char_filename)) std::cout << "Fatal error!";

    AllClasses = owl_api::GetAllClasses(filename);
    AllIndividuals = owl_api::GetAllIndividuals(filename);
    AllObjectProperties = GetAllObjectProperties(filename);
    AllDataProperties = GetAllDataProperties(filename);

    simple_walker_for_inverse_prop walker;
    doc.traverse(walker);
    functional_properties = walker.functional_property;
    inverse_functional_properties = walker.inverse_functional_property;
    simple_walker_for_inverse walker_2;
    doc.traverse(walker_2);
    inverse = walker_2.current_inverse;


    simple_walker_for_subclasses_ext_2 walker_3;
    doc.traverse(walker_3);
    class_super_class = walker_3.sub;

    simple_walker_for_subprop walker_4;
    doc.traverse(walker_4);
    properties_children = walker_4.sub;


    simple_walker_for_restrictions  walker_5;
    doc.traverse(walker_5);

    restrictions = walker_5.data;
    connected_restrictions = walker_5.connected_data;
    connected_restrictions_with_values = walker_5.connected_data_with_values;


    simple_walker_for_individual_value_v2 walker_6;
    doc.traverse(walker_6);

    nameInd_prop_values = walker_6.name_prop_values;


    simple_walker_for_individual_superclasses walker_7;
    doc.traverse(walker_7);

    individual_superclasses = walker_7.individual_superclasses;


    simple_walker_for_equvalent_classes walker_8;
    doc.traverse(walker_8);
    EquivalentClasses = walker_8.EquivalentClasses;


    simple_walker_for_import walker_9;
    doc.traverse(walker_9);


    simple_walker_for_import walker_10;
    doc.traverse(walker_10);
    IRIimports = walker_10.IRIs;

    for(int i = 0; i < IRIimports.size(); ++i)
    {
        if(IRI_to_filenames.find(IRIimports[i]) == IRI_to_filenames.end())
            std::cout << "Warning!! Cannot find file with IRI " << IRIimports[i]<< std::endl;
        else{
            loader tmp;
            tmp.set_filename(IRI_to_filenames[IRIimports[i]]);
            tmp.load();
            add_info(tmp);
        }
    }


    std::set<std::string> tmp_eq_classes;

    std::map <std::string, std::vector <std::string> >::iterator it_eq_classes;

    for(it_eq_classes = EquivalentClasses.begin(); it_eq_classes != EquivalentClasses.end(); ++it_eq_classes)
    {

        tmp_eq_classes = std::set<std::string>(it_eq_classes->second.begin(), it_eq_classes->second.end());
        EquivalentClasses[it_eq_classes->first] = std::vector<std::string>(tmp_eq_classes.begin(), tmp_eq_classes.end());
    }




    std::map <std::string, std::vector <std::pair <std::string, std::string> > >::
    iterator it;

    std::set <std::string > tmp;
    for(it = nameInd_prop_values.begin(); it!= nameInd_prop_values.end(); ++it)
        for(int i = 0; i < it->second.size(); ++i)
    {

        if(property_members.find(it->second[0].first) == property_members.end())
            property_members[it->second[0].first] = std::vector <std::string> ();

        property_members[it->second[0].first].push_back(it->first);



    }

    std::map <std::string, std::vector <std::string> >::
    iterator it2;
    for(it2 = property_members.begin(); it2!= property_members.end(); ++it2)
    {

        tmp = std::set<std::string>(it2->second.begin(), it2->second.end());
        property_members[it2->first] = std::vector<std::string>(tmp.begin(), tmp.end());
    }

}


std::vector<std::string> get_all_eq_classes(std::string start,
                            std::map <std::string, std::vector <std::string> >& eq_classes){

    std::set<std::string> viseted;
    std::stack<std::string> my_stack;
    std::vector<std::string> result;
    if(eq_classes.find(start) == eq_classes.end())
        return std::vector<std::string>();
    else{
        viseted.insert(start);
        for(int i = 0; i < eq_classes[start].size(); ++i)
            {my_stack.push(eq_classes[start][i]);
            viseted.insert(eq_classes[start][i]);
            result.push_back(eq_classes[start][i]);

            }

        while(!my_stack.empty()){
            std::string current = my_stack.top();
            my_stack.pop();
            if(viseted.find(current) == viseted.end() && eq_classes.find(current)!=eq_classes.end())
            {
                for(int i =0; i < eq_classes[current].size(); ++i)
                {
                    my_stack.push(current);
                    result.push_back(current);
                    viseted.insert(current);
                }
            }

        }
        return result;
    }

}

std::vector<std::string> pub_all_eq_classes(std::string a){
    return get_all_eq_classes(a, EquivalentClasses);
}
bool isClass(std::string name){
    if(AllClasses.find(name) != AllClasses.end())
        return true;
    return false;

}

bool isMember(std::string name){
    if(AllIndividuals.find(name) != AllIndividuals.end())
        return true;
    return false;

}

bool isObjectProperty(std::string name){
    if(AllObjectProperties.find(name) != AllObjectProperties.end())
        return true;
    return false;
}

bool isDataProperty(std::string name){
    if(AllDataProperties.find(name) != AllDataProperties.end())
        return true;
    return false;

}

bool IsFunctionalProperty(std::string name){
    if(functional_properties.find(name) != functional_properties.end())
        return true;
    return false;
}

bool IsInverseFunctionalProperty(std::string name){
    if(inverse_functional_properties.find(name) != inverse_functional_properties.end())
        return true;
    return false;
}

bool HasInverseProperty(std::string name){
    if(inverse.find(name) != inverse.end() ||
       inverse_functional_properties.find(name) != inverse_functional_properties.end())
        return true;
    return false;

}

std::vector<std::string> GetSuperClasses (std::string name){
    std::vector <std::string> tmp =  get_all_parents(class_super_class, name);
    std::vector <std::string> res =  get_all_parents(class_super_class, name);
    for(int i =0; i < tmp.size(); ++i)
    {
        std::vector<std::string> eq = pub_all_eq_classes(tmp[i]);
        for(int j =0; j < eq.size();++j )
            res.push_back(eq[j]);

    }
    res.push_back("Thing");

    return res;

}

std::vector<std::string> GetMembersWithProperty_old(std::string property, std::string value){
    std::vector<std::string>  result;

    char * char_filename = new char [filename.length()+1];
    strcpy (char_filename, filename.c_str());
    pugi::xml_document doc;
    if (!doc.load_file(char_filename)) std::cout << "Fatal error!";
    simple_walker_for_individual_value walker;

    walker.property = property;
    walker.value = value;
    walker.set_prop();

    doc.traverse(walker);

    return walker.result;
}

std::vector <std::string> GetSubProperties(std::string prop){

    return all_children_subprop(properties_children, prop);
}

bool CheckPropertyForClass(std::string class_, std::string property, std::string value = "@@")
{

    if(value == "@@")
        {
            std::vector<std::string> super_cl = GetSuperClasses(class_);
            super_cl.push_back(class_);
            bool result = false;
            for(int i =0; i < super_cl.size(); ++i){


                    if(connected_restrictions.find(super_cl[i] + " " + property)!=
                connected_restrictions.end())
                    result = true;}

            return result;

        }

    else{

        if(AllClasses.find(value) == AllClasses.end())
            return connected_restrictions_with_values.find(class_ + " " + property + " " + value)!=
                connected_restrictions_with_values.end();
        else{
            std::vector<std::string> super_cl = GetSuperClasses(value);
            super_cl.push_back(value);
            bool result = false;
            for(int i =0; i < super_cl.size(); ++i){


                    if(connected_restrictions_with_values.find(class_ + " " + property + " " +super_cl[i])!=
                       connected_restrictions_with_values.end())
                result = true;
            }

            return result;



            }
        }

}

bool CheckPropertyForMember_old(std::string member_name, std::string property, std::string value = "%%%")
{

    char * char_filename = new char [filename.length()+1];
    strcpy (char_filename, filename.c_str());
    pugi::xml_document doc;
    if (!doc.load_file(char_filename)) std::cout << "Fatal error!";
    simple_walker_for_CheckPropertyForMember walker;

    walker.property = property;

    walker.set_prop();
    //walker.name_of_the_object = "owl:Class";
    doc.traverse(walker);
    bool res = false;

    if(walker.result.find(member_name) == walker.result.end())
        return false; //unobvious
    else{

        if(value == "%%%")
            res = true;
        else{
        for(int i = 0; i < walker.result[member_name].size();++i)
           if(walker.result[member_name][i] == value)
                res = true;
        }
    }
    return res;
}



bool CheckPropertyForMember(std::string member_name, std::string property,
                                std::string value = "%%%")
{
     if(nameInd_prop_values.find(member_name) == nameInd_prop_values.end())
        return false;
     else{
        std::vector <std::pair <std::string, std::string> > tmp =
        nameInd_prop_values[member_name];
        if(tmp.size() == 0)
            return false;

        else{
            for(int i =0; i < tmp.size(); ++i){
                if(value == "%%%" && tmp[i].first == property)
                    return true;
                else
                    if(tmp[i].first == property && tmp[i].second == value)
                        return true;
                }
            }
        }

    return false;


}
std::string GetDatalogSubclassRules(){
    //return string.
    std::map<std::string, std::vector <std::string> >::iterator it;
    std::vector <std::string>::iterator i;
    std::string result;
    for(it = class_super_class.begin(); it!= class_super_class.end(); ++it)
    {
        std::string tmp;
        for( int i = 0; i  < it->second.size(); ++i)
        {

            //bool z = class_super_class.find(it->second[i]) == class_super_class.end();
            //cout << z;

            if(class_super_class.find(it->second[i]) == class_super_class.end())
                tmp = "owl:" + it->second[i];
            else
                tmp = "common:";

            tmp = tmp + it->second[i] + "(?x):-" + "common:"  + it->first + "(?x)\n";

        }
        result = result + tmp;
    }

    return result;
}


std::string GetDatalogInversePropertyRules(){
    std::map<std::string, std::string>::iterator it;

    std::string result;
    for(it = inverse.begin(); it!=inverse.end(); ++it)
    {
        std::string tmp;
        tmp = "common:" +  it->first + "(?y, ?x) :- " + "common:" +
            it->second + "(?x, ?y).\n";
        result += tmp;
    }
    return result;
}


std::string GetDatalogFunctionalPropertyRules(){
     std::set<std::string>::iterator it;
     std::string result;
     std::string tmp;
     for(it = inverse_functional_properties.begin();
         it != inverse_functional_properties.end();
         ++it)
         {
             if(functional_properties.find(*it) != functional_properties.end())
                tmp = "owl:sameAs(?A1, ?A2) :- common: " +
                *it + "(?X, ?A1), common: "  +  *it + "(?Y, ?A2), owl:sameAs(?X, ?Y)\n";
                result += tmp;
         }

    return result;
}
void debug_print_a_super_class(std::string name){


 std::vector <std::string> ::iterator it;
std::cout << class_super_class[name].size() << std::endl;
 for(it = class_super_class[name].begin(); it!= class_super_class[name].end(); ++it)
    std::cout << *it << " ";
}



std::vector<std::string> GetRelNames(){
    std::vector <std::string> result;
    for(std::set<std::string>::iterator it = AllObjectProperties.begin();
        it != AllObjectProperties.end(); ++ it)
        result.push_back(*it);

    for(std::set<std::string>::iterator it = AllDataProperties.begin();
        it != AllDataProperties.end(); ++it)
        if(*it!= "hasValue")
            result.push_back(*it);

    result.push_back("topObjectProperty");
    result.push_back("topDataProperty");

    return result;

}


bool BelongToClass(std::string one, std::string two){
    if(AllClasses.find(one) != AllClasses.end() ){
        std::vector<std::string> tmp = GetSuperClasses(one);
        std:: set<std::string> tmp_2(tmp.begin(), tmp.end());

        for(std::set<std::string>::iterator it =tmp_2.begin();
        it!=tmp_2.end(); ++it){
        std::vector<std::string> eq_cl = pub_all_eq_classes(*it);
        for(int i =0; i < eq_cl.size(); ++i)
            tmp_2.insert(eq_cl[i]);

        }


        return tmp_2.find(two) != tmp_2.end();
    }

    if(AllIndividuals.find(one)!= AllIndividuals.end()){
        if(individual_superclasses.find(one) == individual_superclasses.end())
            return false;
        else{
            std::vector<std::string> isc_1 = individual_superclasses[one];
            std::set<std::string> supers;
            std::vector<std::string> tmp;
            for(int i =0; i < isc_1.size(); ++i){
                supers.insert(isc_1[i]);
                tmp = GetSuperClasses(isc_1[i]);
                for(int j =0; j < tmp.size(); ++j)
                    {supers.insert(tmp[j]);
                    }
            }


            return supers.find(two) != supers.end();
        }


    }

}
std::vector <std::string> GetMembersWithProperty(std::string prop, std::string value){
    std::map <std::string, std::vector <std::pair <std::string, std::string> > >::iterator it;
    std::vector<std::string> result;
    for(it = nameInd_prop_values.begin(); it!= nameInd_prop_values.end(); ++it)
    {
        bool tmp = false;
        for(int i =0; i< nameInd_prop_values[it->first].size();++i){
            if(nameInd_prop_values[it->first][i].first == prop &&
               nameInd_prop_values[it->first][i].second == value)
                tmp = true;

        }

        if(tmp)
            result.push_back(it->first);
    }

    return result;


}

std::vector <std::string> GetAllClasses(){
    std::vector<std::string> tmp = std::vector<std::string>(AllClasses.begin(), AllClasses.end());
    tmp.push_back("Thing");
    return  tmp;

}

std::vector <std::string>  GetAllIndividuals(){
    return std::vector<std::string>(AllIndividuals.begin(), AllIndividuals.end());
}


};
}
#endif // OWL_API
