#include "ts.h"

#define NB_REGISTRE  9

/**
 * @brief Recherche un identifiant dans la table des symboles.
 *
 * Cette fonction parcourt la table des symboles pour localiser un identifiant
 * dans un contexte donné. Si l'identifiant n'est pas trouvé dans le contexte local,
 * il est recherché dans le contexte global.
 *
 * @param tabsymb Table des symboles à parcourir.
 * @param context Contexte dans lequel rechercher l'identifiant.
 * @param id Identifiant à rechercher.
 * @return L'indice de l'identifiant dans la table des symboles ou une erreur si introuvable.
 */
int ts_recherche_id(ts tabsymb,char* context, char* id){
    int i = 0;
    int var_global = -1; //recupere l'adresse de l'id dans le contexte global si elle existe
    while (i<TAILLE_MAX_TS){
        if (strcmp(tabsymb[i].context,context) == 0){
            if (strcmp(tabsymb[i].id,id) == 0){
                if(tabsymb[i].type == 0){
                    return i;
                }
            }
        }
        if (strcmp(tabsymb[i].context,"GLOBAL") == 0){
            if (strcmp(tabsymb[i].id,id) == 0){
                
                if(tabsymb[i].type == 0){
                    var_global = i;
                }
            }     
        }
    

    i++;
    }
    if (var_global != -1){
        return var_global;
    }
    if (i==TAILLE_MAX_TS){
        erreur(NAME_ERROR,"Erreur dans la recherche d'id dans la table de symbole : Soit '%s' n'est déclaré ni en GLOBAL ni en LOCAL. Soit il n'y a plus de place dans la table de symbole\n",id);
    }
    return -1;
}

/*
On ne peut créer et chercher un fonction que dans le contexte global
Il est donc impossible de créer des fonction dans des fonction
*/
/**
 * @brief Recherche une fonction dans la table des symboles.
 *
 * Cette fonction vérifie si une fonction donnée est déclarée dans le contexte global
 * de la table des symboles.
 *
 * @param tabsymb Table des symboles à parcourir.
 * @param id Identifiant de la fonction à rechercher.
 * @return L'indice de la fonction dans la table des symboles ou une erreur si introuvable.
 */
int ts_recherche_fonction(ts tabsymb, char* id){ 
    int i = 0;
    while (i<TAILLE_MAX_TS){
        if (strcmp(tabsymb[i].context,"GLOBAL") == 0){
            if (strcmp(tabsymb[i].id,id) == 0){
                if (tabsymb[i].type == 1){
                    return i;
                }
            }
        }  
    i++;
    }
    if (i==TAILLE_MAX_TS){

        erreur(NAME_ERROR,"Erreur dans la recherche de fonction dans la table de symbole : Soit '%s' n'est déclaré. Soit il n'y a plus de place dans la table de symbole\n",id);
    }
    return -1;
}

/**
 * @brief Ajoute une nouvelle fonction à la table des symboles.
 *
 * Cette fonction insère une déclaration de fonction dans le contexte global de la
 * table des symboles tout en vérifiant les doublons.
 *
 * @param tabsymb Table des symboles où ajouter la fonction.
 * @param id Identifiant de la fonction à ajouter.
 * @return L'indice de la fonction ajoutée ou une erreur si la table est pleine.
 */
int ts_ajouter_fonction(ts tabsymb, char* id){
    int i = 0;
    while (i<TAILLE_MAX_TS && tabsymb[i].id[0] != '\0'){      
        if (tabsymb[i].type == 1){
            if (strcmp(tabsymb[i].id,id) == 0){
            erreur(REDEFINITION_ERROR,"La fonction '%s' est deja declarée\n",id);
        }
        }
        i++;
    }
    if (i == TAILLE_MAX_TS){
        erreur(MEMORY_ERROR,"Erreur dans l'ajout de fonction: plus de place dans la table de symbole");
        return -1;
    }
    else{
        strcpy(tabsymb[i].context,"GLOBAL");
        strcpy(tabsymb[i].id,id);
        tabsymb[i].type = 1; //TODO Choisir une valeur pour le type
        tabsymb[i].adresse = 0;
        return i;
    }
}

//ajoute un identifiant dans la table de symbole au contexte donnée
/**
 * @brief Ajoute une nouvelle variable à la table des symboles.
 *
 * Cette fonction insère une déclaration d'identifiant (variable) dans un contexte donné
 * de la table des symboles tout en vérifiant les doublons.
 *
 * @param tabsymb Table des symboles où ajouter l'identifiant.
 * @param context Contexte dans lequel ajouter l'identifiant.
 * @param id Identifiant à ajouter.
 * @return L'indice de l'identifiant ajouté ou une erreur si la table est pleine.
 */
int ts_ajouter_id(ts tabsymb,char* context, char* id){
    int i = 0;
    int valeur_adr = 0;
    while (i<TAILLE_MAX_TS && tabsymb[i].id[0] != '\0'){      
        if (strcmp(tabsymb[i].context,context) == 0){
            valeur_adr++;
            if (strcmp(tabsymb[i].id,id) == 0){
            erreur(REDEFINITION_ERROR,"La variable '%s' est deja declarée\n",id);
            }
        }
        i++;
    }
    if (i == TAILLE_MAX_TS){
        erreur(MEMORY_ERROR,"Erreur dans l'ajout d'id: plus de place dans la table de symbole");
    }

    strcpy(tabsymb[i].context,context);
    strcpy(tabsymb[i].id,id);
    tabsymb[i].type = 0;
    tabsymb[i].adresse = valeur_adr;
    return i;
    
}


//mise en forme fait par chatgpt
/**
 * @brief Affiche la table des symboles.
 *
 * Cette fonction imprime dans un format lisible l'intégralité de la table des symboles,
 * incluant les détails sur les variables et les fonctions, ainsi que leurs propriétés.
 *
 * @param tabsymb Table des symboles à afficher.
 */
void PrintTS(ts tabsymb) {
    printf("======================== Table des Symboles ========================\n");
    printf("%-5s | %-10s | %-15s | %-10s | %-10s\n", 
           "Index", "Contexte", "Nom", "Adresse", "Type");
    printf("====================================================================\n");

    for (int i = 0; i < TAILLE_MAX_TS && tabsymb[i].id[0] != '\0'; i++) {
        printf("%-5d | %-10s | %-15s | %-10d | %-10d\n", 
               i + 1, 
               tabsymb[i].context, 
               tabsymb[i].id, 
               tabsymb[i].adresse, 
               tabsymb[i].type);

        if (tabsymb[i].type == 1) {
            printf("      > Début fonction: %d  /  Renvoie: %s\n",
                   tabsymb[i].debut, 
                   tabsymb[i].Possede_renvoie ? "Oui" : "Non");
        }
    }

    printf("====================================================================\n");
}
