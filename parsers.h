#ifndef PARSERS.H
#define PARSERS.H
 // PARSERS

#include "pugixml.hpp"
#include <iostream>
#include<vector>
#include<string>
#include<set>
#include<map>
#include<stack>
#include <stdio.h>
#include<string.h>


const char* node_types[] =
{
    "null", "document", "element", "pcdata", "cdata", "comment", "pi", "declaration"
};


// tag::impl[]
struct simple_walker: pugi::xml_tree_walker
{
    std::vector <std::string> data;

    virtual bool for_each(pugi::xml_node& node)
    {
        for (int i = 0; i < depth(); ++i) std::cout << "  "; // indentation

        std::cout << ": name='" << node.name() << "', value='" << node.value() << "'\n";
        std::cout << node.attribute("rdf:about");
        std::cout << node.child("rdfs:subClassOf").attribute("rdf:resource").value();
        if(node.attribute("rdf:about"))
            data.push_back(node.attribute("rdf:about").value());
        return true; // continue traversal
    }
};
// end::impl[]
struct simple_walker_template: pugi::xml_tree_walker
{
    std::vector<std::string> names;
    std::string name_of_the_object;
    virtual bool for_each(pugi::xml_node& node)
    {
         // indentation


        std::string str(node.name());
        if(str == name_of_the_object)
            names.push_back(node.attribute("rdf:about").value());
        return true; // continue traversal
    }
};


std::string get_name_from_string_about(std::string about){
    int s = about.find('#');
    std::string t = about.substr(s+1);
    return t;
}

std::set<std::string> get_all_something_names(std::string something,
                                              std::string filename){


    char * char_filename = new char [filename.length()+1];
    strcpy (char_filename, filename.c_str());


    pugi::xml_document doc;
    if (!doc.load_file(char_filename))
        std::cout << "ERROR!! File with ontology was not loaded";
    simple_walker_template walker;

    walker.name_of_the_object = something;
    doc.traverse(walker);

    std::set <std::string> result;
    for(int i = 0; i < walker.names.size(); ++i)
        result.insert(
         get_name_from_string_about(walker.names[i]));

    return result;
}


struct simple_walker_for_subclasses: pugi::xml_tree_walker
{
    std::map<std::string, std::string> sub;
    virtual bool for_each(pugi::xml_node& node)
    {


        if(node.child("rdfs:subClassOf").attribute("rdf:resource"))
            {
            std::string  current_name;
            std::string name_of_super_class;

        /***
        Warning! We take only one superclass. We  must take everything!
        ***/
             current_name = std::string(node.attribute("rdf:about").value());
             name_of_super_class =
                    std::string(node.child("rdfs:subClassOf").attribute("rdf:resource").value());

            current_name =  get_name_from_string_about(
                                current_name);
            name_of_super_class = get_name_from_string_about(
                                name_of_super_class);
            sub[current_name] = name_of_super_class;
            std::cout << current_name <<" " <<name_of_super_class<< std::endl;

            }
        return true; // continue traversal
    }
};


struct simple_walker_for_individual_value: pugi::xml_tree_walker
{
    std::vector< std::string > result;

    std::string property;
    std::string value;



    char * char_property;
    virtual void set_prop(){
        char_property = new char [property.length()+1];
        strcpy (char_property, property.c_str());
    }
    virtual bool for_each(pugi::xml_node& node)
    {



        std::string current_name;
        std::string node_name = node.name();

        if(node_name == "owl:NamedIndividual"){
             current_name = std::string(node.attribute("rdf:about").value());
             current_name =  get_name_from_string_about(current_name);

             bool tmp = false;
                std::string current_value;
                for (pugi::xml_node tool = node.child(char_property); tool; tool = tool.next_sibling(char_property))
                {

                    current_value =  std::string(tool.attribute("rdf:resource").value());
                    current_value = get_name_from_string_about(current_value);

                    if(current_value  == value)
                        tmp = true;
                }

            if(tmp)
                result.push_back(current_name);



        }
            //sub[current_name] = name_of_super_class;



        return true; // continue traversal
    }
};



struct simple_walker_for_individual_value_v2: pugi::xml_tree_walker
{

    std::map <std::string, std::vector <std::pair <std::string, std::string> > >
    name_prop_values;



    virtual bool for_each(pugi::xml_node& node)
    {



        std::string current_name;
        std::string node_name = node.name();
        std::string attr_value;
        if(node_name == "owl:NamedIndividual"){
             current_name = std::string(node.attribute("rdf:about").value());
             current_name =  get_name_from_string_about(current_name);
             name_prop_values[current_name] =  std::vector <std::pair <std::string, std::string> > ();
                for (pugi::xml_node tool = node.first_child(); tool; tool = tool.next_sibling())
                {
                    if(std::string(tool.name()).find(":") == -1)
                    for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute())
                    {

                        attr_value = get_name_from_string_about(attr.value());
                        name_prop_values[current_name].push_back(std::make_pair(tool.name(), attr_value));

                    }


                }

        }
            //sub[current_name] = name_of_super_class;



        return true; // continue traversal
    }
};




struct simple_walker_for_individual_superclasses: pugi::xml_tree_walker
{

    std::map <std::string, std::vector <std::string> >
    individual_superclasses;



    virtual bool for_each(pugi::xml_node& node)
    {



        std::string current_name;
        std::string node_name = node.name();
        std::string attr_value;
        if(node_name == "owl:NamedIndividual"){
             current_name = std::string(node.attribute("rdf:about").value());
             current_name =  get_name_from_string_about(current_name);
             for (pugi::xml_node tool = node.child("rdf:type"); tool; tool = tool.next_sibling("rdf:type"))
                {

                        attr_value = std::string( get_name_from_string_about(tool.attribute("rdf:resource").value()));
                        if(individual_superclasses.find(current_name) == individual_superclasses.end())
                            individual_superclasses[current_name] = std::vector<std::string>();
                        individual_superclasses[current_name].push_back(attr_value);
                }

        }           //sub[current_name] = name_of_super_class;
        return true; // continue traversal
    }
};





struct simple_walker_for_equvalent_classes: pugi::xml_tree_walker
{

    std::map <std::string, std::vector <std::string> >
    EquivalentClasses;



    virtual bool for_each(pugi::xml_node& node)
    {
        std::string current_name;
        std::string node_name = node.name();
        std::string attr_value;
        if(node_name == "owl:Class"){
             current_name = std::string(node.attribute("rdf:about").value());
             current_name =  get_name_from_string_about(current_name);
             for (pugi::xml_node tool = node.child("owl:equivalentClass"); tool; tool = tool.next_sibling("owl:equivalentClass"))
                {

                        attr_value = std::string( get_name_from_string_about(tool.attribute("rdf:resource").value()));
                        if(EquivalentClasses.find(current_name) == EquivalentClasses.end())
                            EquivalentClasses[current_name] = std::vector<std::string>();
                        EquivalentClasses[current_name].push_back(attr_value);

                        if(EquivalentClasses.find(attr_value) == EquivalentClasses.end())
                            EquivalentClasses[attr_value] = std::vector<std::string>();
                        EquivalentClasses[attr_value].push_back(current_name);

                }

        }           //sub[current_name] = name_of_super_class;
        return true; // continue traversal
    }
};


struct simple_walker_for_CheckPropertyForMember: pugi::xml_tree_walker
{
    std::map< std::string, std::vector <std::string > > result;

        char * char_property;
        std::string property;
        virtual void set_prop(){
            char_property = new char [property.length()+1];
            strcpy (char_property, property.c_str());
        }

        virtual bool for_each(pugi::xml_node& node)
        {


        std::string current_name;
        std::string node_name = node.name();

        if(node_name == "owl:NamedIndividual"){
             current_name = std::string(node.attribute("rdf:about").value());
             current_name =  get_name_from_string_about(current_name);
             result[current_name] = std::vector<std::string>();

                std::string current_value;
                for (pugi::xml_node tool = node.child(char_property); tool; tool = tool.next_sibling(char_property))
                {
                    current_value =  std::string(tool.attribute("rdf:resource").value());
                    current_value = get_name_from_string_about(current_value);
                    result[current_name].push_back(current_value);


                }


        }
         return true;
        }
            //sub[current_name] = name_of_super_class;



       // continue traversal

};







struct simple_walker_for_subclasses_ext_2: pugi::xml_tree_walker
{
    std::map<std::string, std::vector <std::string> > sub;

    virtual bool for_each(pugi::xml_node& node)
    {


        if(node.child("rdfs:subClassOf").attribute("rdf:resource"))
            {
            std::string  current_name;
            std::string name_of_super_class;


             current_name = std::string(node.attribute("rdf:about").value());
             current_name =  get_name_from_string_about(current_name);
             std::vector<std::string> tmp;

                for (pugi::xml_node tool = node.child("rdfs:subClassOf"); tool; tool = tool.next_sibling("rdfs:subClassOf"))
                {

                    name_of_super_class =  std::string( tool.attribute("rdf:resource").value());
                    name_of_super_class = get_name_from_string_about(
                                name_of_super_class);
                    if(name_of_super_class.length() > 0)
                        tmp.push_back(name_of_super_class);
                }
                sub[current_name] = tmp;



            //sub[current_name] = name_of_super_class;


            }
        return true; // continue traversal
    }
};


class restriction{
public:
    std::string class_name;
    std::string name_of_restriction;
    std::string value_of_restriction;
    std::string type_of_restriction;

    restriction(std::string class_name_, std::string name_of_restriction_,
                std::string value_of_restriction_, std::string type_of_restriction_){

        class_name = class_name_;
        name_of_restriction = name_of_restriction_;
        value_of_restriction = value_of_restriction_;
        type_of_restriction = type_of_restriction_;


    }

    void print(){

    std::cout << class_name << " "<< type_of_restriction << " " << name_of_restriction << " " << value_of_restriction;
    }


};


struct simple_walker_for_restrictions: pugi::xml_tree_walker
{

    std::vector <restriction> data;
    std::set <std::string> connected_data;
    std::set <std::string> connected_data_with_values;


    virtual bool for_each(pugi::xml_node& node)
    {


        if(node.child("rdfs:subClassOf"))
            {
            std::string  current_name;
            std::string name_of_super_class;


            current_name = std::string(node.attribute("rdf:about").value());
            current_name =  get_name_from_string_about(current_name);

            std::string on_property;

            std::string name_of_restriction;
            std::string value_of_restriction;
            std::string type_of_restriction;
            for (pugi::xml_node tool = node.child("rdfs:subClassOf"); tool; tool = tool.next_sibling("rdfs:subClassOf"))
            {


                if(tool.child("owl:Restriction"))
                {
                     for (pugi::xml_node tool2 = tool.child("owl:Restriction"); tool2; tool2 = tool.next_sibling("owl:Restriction"))
                        {

                            //cout << current_name << " "<< tool2.child("owl:onProperty").attribute("rdf:resource").value()<<endl;
                            if(!tool2.child("owl:onProperty"))
                                std::cout << "error " <<current_name <<std:: endl;
                            else
                            {
                                name_of_restriction = tool2.child("owl:onProperty").attribute("rdf:resource").value();
                                name_of_restriction = get_name_from_string_about(name_of_restriction);

                                if(tool2.child("owl:someValuesFrom"))
                                    {type_of_restriction = "owl:someValuesFrom";

                                    }

                                if(tool2.child("owl:hasValue"))
                                    {type_of_restriction = "owl:hasValue";
                                    }
                                if(tool2.child("owl:allValuesFrom"))
                                   {

                                    type_of_restriction = "owl:allValuesFrom";
                                   }


                                   const char *cstr = type_of_restriction.c_str();
                                   value_of_restriction = tool2.child(cstr).attribute("rdf:resource").value();
                                   value_of_restriction = get_name_from_string_about(value_of_restriction);


                                   data.push_back(restriction(current_name, name_of_restriction,
                                                              value_of_restriction, type_of_restriction));

                                    connected_data.insert(current_name + " " + name_of_restriction);
                                    connected_data_with_values.insert(current_name + " " + name_of_restriction + " " +
                                                                      value_of_restriction);
                            }

                        }

                }


            //sub[current_name] = name_of_super_class;


            }
        }
        return true; // continue traversal
    }
};




struct simple_walker_for_inverse_prop: pugi::xml_tree_walker
{
    std::set<std::string> functional_property;
    std::set<std::string> inverse_functional_property;

    virtual bool for_each(pugi::xml_node& node)
    {
        std::string str(node.name());
        std::string current_name = std::string(node.attribute("rdf:about").value());
        current_name =  get_name_from_string_about(current_name);

        if(str == "owl:ObjectProperty" || str == "owl:DatatypeProperty")
            {
                for (pugi::xml_node tool = node.child("rdf:type"); tool; tool = tool.next_sibling("rdf:type"))
                {

                    std::string current_string =  std::string( tool.attribute("rdf:resource").value());
                    current_string = get_name_from_string_about(current_string);

                    if(current_string == "FunctionalProperty")
                        functional_property.insert(current_name);

                    if(current_string == "InverseFunctionalProperty")
                        {inverse_functional_property.insert(current_name);
                        //std::cout << current_name << " ";

                        };

                }
            //sub[current_name] = name_of_super_class;
            }



        return true; // continue traversal
    }
};

struct simple_walker_for_subclasses_ext: pugi::xml_tree_walker
{
    std::map<std::string, std::string> sub;
    virtual bool for_each(pugi::xml_node& node)
    {


        if(node.child("rdfs:subClassOf").attribute("rdf:resource"))
            {
                for (pugi::xml_node tool = node.first_child(); node; tool = tool.next_sibling())
                {
                    std::cout << "Tool:";

                    for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute())
                    {
                        std::cout << " " << attr.name() << "=" << attr.value();
                    }

                    std::cout << std::endl;
                }

            }
        return true; // continue traversal
    }
};
//This is violation of dry princepe. must be refactored.


struct simple_walker_for_subprop: pugi::xml_tree_walker
{
    std::map<std::string, std::vector <std::string> > sub;

    virtual bool for_each(pugi::xml_node& node)
    {


        if(node.child("rdfs:subPropertyOf").attribute("rdf:resource"))
            {
            std::string  current_name;
            std::string name_of_super_prop;


             current_name = std::string(node.attribute("rdf:about").value());
             current_name =  get_name_from_string_about(current_name);

                for (pugi::xml_node tool = node.child("rdfs:subPropertyOf"); tool; tool = tool.next_sibling("rdfs:subPropertyOf"))
                {

                    name_of_super_prop =  std::string( tool.attribute("rdf:resource").value());
                    name_of_super_prop = get_name_from_string_about( name_of_super_prop);

                    if(sub.find(name_of_super_prop) == sub.end())
                        sub[name_of_super_prop] = std::vector <std::string>();
                    sub[name_of_super_prop].push_back(current_name);
                }




            //sub[current_name] = name_of_super_class;


            }
        return true; // continue traversal
    }
};



//This is violation of dry princepe. must be refactored.
struct simple_walker_for_inverse: pugi::xml_tree_walker
{
    std::map<std::string, std::string> current_inverse;
    virtual bool for_each(pugi::xml_node& node)
    {


        if(node.child("owl:inverseOf").attribute("rdf:resource")){
            std::string  current_name;
            std::string name_of_inverse;


            current_name = std::string(node.attribute("rdf:about").value());
            name_of_inverse =
                    std::string(node.child("owl:inverseOf").attribute("rdf:resource").value());

            current_name =  get_name_from_string_about(
                                current_name);
            name_of_inverse = get_name_from_string_about(
                                name_of_inverse);
            current_inverse[current_name] = name_of_inverse;
            current_inverse[name_of_inverse] = current_name;


            }
        return true; // continue traversal
    }
};


struct simple_walker_for_import: pugi::xml_tree_walker{

    std::vector <std::string> IRIs;
    virtual bool for_each(pugi::xml_node& node)
    {
            std::string current_name = std::string(node.name());

            if(current_name == "owl:Ontology")
            {
              for (pugi::xml_node tool = node.child("owl:imports"); tool;
                   tool = tool.next_sibling("owl:imports")){
                   IRIs.push_back(std::string(tool.attribute("rdf:resource").value()));
                   std::cout << std::string(tool.attribute("rdf:resource").value()) << " ";
                   }

            }
        return true;
    }


};


#endif
