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
#include "owl_api.h"


/**
void test_look_GetMembersWithProperty(std::string property = "isMeetingSpatially"){
    owl_api::loader t;
    t.set_filename("ontoetap.owl");
    t.load();
    std::vector<std::string> tt = t.GetMembersWithProperty(property);

    for(int i =0; i < tt.size(); ++i)
        std::cout << tt[i]<< std::endl;

}
*/

void test_look_GetRelNames(){
    owl_api::loader t;
    t.set_filename("ontoetap.owl");
    t.load();
    std::vector <std::string> result = t.GetRelNames();
    for(int i = 0; i < result.size(); ++i)
        std::cout << result[i] << std::endl;

}


void test_look_GetDatalogFunctionalPropertyRules(){
    owl_api::loader t;
    t.set_filename("ontoetap.owl");
    t.load();
    std::cout << t.GetDatalogFunctionalPropertyRules();
}


void  test_look_GetSubProperties() {

    owl_api::loader t;
    t.set_filename("ontoetap.owl");
    t.load();

    std::vector<std::string> res  = t.GetSubProperties("SUMOpredicate");

    for(int i = 0; i < res.size(); ++i)
        std::cout << res[i] << std::endl;
}

int main()
{



    pugi::xml_document doc;
    if (!doc.load_file("ontoetap.owl")) return -1;

    owl_api::loader t;
    t.set_filename("littleonto.owl");
    t.map_ontology_IRI("http://proling.iitp.ru/ontologies/ontoetap.owl", "ontoetap.owl");
    //t.set_filename("ontoetap.owl");
    t.load();
    std::cout << t.isClass("Human");


    //std::cout << t.CheckPropertyForClass("Buying", "hasAgent", "Human");
    //std::cout << t.HasInverseProperty("isAgentOf")<< " "<< t.HasInverseProperty("hasAgent");

/**
    std::vector<std::string> res = t.GetAllIndividuals();
    for(size_t i = 0; i < res.size(); ++i)
        std::cout << res[i] << " ";
*/



    return 0;

}

// vim:et
