#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <string.h>


#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900
#define FONT_SIZE 32
#define TITLE_TEXT "N-ary Tree Menu"
#define TITLE_FONT_SIZE 40


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
Mix_Music* music;
Mix_Chunk *click ,*hover;
SDL_Surface *ScreenSurface,* background;

typedef struct {
    SDL_Rect rect;
    SDL_Color color;
    const char* text;
    int hoverFlag;
} MenuItem;

typedef struct Node {
    int data;
    struct Node** children;
    int childCount;
} Node;

// Function to initialize a node with data
Node* createNode(int data, int numChildren) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->childCount = numChildren;

    if (numChildren > 0) {
        newNode->children = (Node**)malloc(numChildren * sizeof(Node*));
        for (int i = 0; i < numChildren; i++) {
            newNode->children[i] = NULL;  // Initialize children pointers to NULL
        }
    } else {
        newNode->children = NULL;
    }

    return newNode;
}

void deleteNode(Node* root, int target) {
    if (root == NULL) {
        return;
    }

    // Check if the root contains the target value
    if (root->data == target) {
        free(root);
        return;
    }

    // Iterate through the children and recursively delete nodes with the target value
    for (int i = 0; i < root->childCount; i++) {
        if (root->children[i]->data == target) {
            // Recursively delete the node and its descendants
            deleteNode(root->children[i], target);
            // Free the memory for the current child
            free(root->children[i]);

            // Remove the child from the array and adjust the count
            for (int j = i; j < root->childCount - 1; j++) {
                root->children[j] = root->children[j + 1];
            }
            root->childCount--;

            // Decrement i to recheck the current index after adjustment
            i--;
        } else {
            // Recursively check the child and its descendants
            deleteNode(root->children[i], target);
        }
    }
}
// Recursive function for creating children within the SDL event loop
void createChildrenSDL(Node* parent, SDL_Renderer* renderer, SDL_Window* window, TTF_Font* font) {
    SDL_Rect inputTextRect = {200, 350, 0, 0};  // Declare outside the loop
    SDL_Rect inputRect = {200, 300, 0, 0};
    
    for (int i = 0; i < parent->childCount; i++) {
        int childData, numGrandchildren;

        // Combine prompt and user input text for the value of the child
        char combinedText[200];

        // Render combined text
        SDL_Color textColor = {255, 255, 255};
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, combinedText, textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {inputTextRect.x, inputTextRect.y, 0, 0};

        // Render the text to the SDL window
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_RenderPresent(renderer);

        // Handle SDL events for input
        SDL_Event event;
        char inputText[100] = "";

        while (1) {
            SDL_PollEvent(&event);

            if (event.type == SDL_QUIT) {
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                TTF_Quit();
                SDL_Quit();
                return;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN) {
                    break;
                } else if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0) {
                    inputText[strlen(inputText) - 1] = '\0';
                } else if (event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) {
                    SDL_SetClipboardText(inputText);
                } else if (event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
                    strncpy(inputText, SDL_GetClipboardText(), sizeof(inputText) - 1);
                }
            } else if (event.type == SDL_TEXTINPUT) {
                strcat(inputText, event.text.text);
            }

        // Render text to the SDL window
        SDL_RenderClear(renderer);

       SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, background);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
        SDL_DestroyTexture(backgroundTexture);

        // Combine prompt and user input text
        sprintf(combinedText, "%s %s", "Enter the value for the root of the tree:", inputText);

        // Render combined text
        textSurface = TTF_RenderText_Solid(font, combinedText, textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        // Render combined text
        SDL_Rect inputTextRect = {inputRect.x, inputRect.y + inputRect.h, 0, 0};
        SDL_QueryTexture(textTexture, NULL, NULL, &inputTextRect.w, &inputTextRect.h);
        SDL_RenderCopy(renderer, textTexture, NULL, &inputTextRect);

        SDL_RenderPresent(renderer);

        // Cleanup
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        }

        // Convert user input to integer
        childData = atoi(inputText);

        // Render combined text for number of grandchildren
        inputTextRect.y += textRect.h + 10;
        inputText[0] = '\0';

        while (1) {
            SDL_PollEvent(&event);

            if (event.type == SDL_QUIT) {
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                TTF_Quit();
                SDL_Quit();
                return;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN) {
                    break;
                } else if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0) {
                    inputText[strlen(inputText) - 1] = '\0';
                } else if (event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) {
                    SDL_SetClipboardText(inputText);
                } else if (event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
                    strncpy(inputText, SDL_GetClipboardText(), sizeof(inputText) - 1);
                }
            } else if (event.type == SDL_TEXTINPUT) {
                strcat(inputText, event.text.text);
            }

       SDL_RenderClear(renderer);
         

    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, background);
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    SDL_DestroyTexture(backgroundTexture);
        // Combine prompt and user input text
        sprintf(combinedText, "%s %s", "Enter the number of children for the root (0 to terminate):", inputText);

        // Render combined text
        textSurface = TTF_RenderText_Solid(font, combinedText, textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        // Render combined text
        SDL_Rect inputTextRect = {inputRect.x, inputRect.y + inputRect.h, 0, 0};
        SDL_QueryTexture(textTexture, NULL, NULL, &inputTextRect.w, &inputTextRect.h);
        SDL_RenderCopy(renderer, textTexture, NULL, &inputTextRect);

        SDL_RenderPresent(renderer);

        // Cleanup
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        }

        // Convert user input to integer for number of grandchildren
        numGrandchildren = atoi(inputText);

        parent->children[i] = createNode(childData, numGrandchildren);
        createChildrenSDL(parent->children[i], renderer, window, font);  // Recursive call to create children of the current child
    }
}




// Function to create a sample tree (you may need to customize this)
Node* createSampleTree(SDL_Renderer* renderer, SDL_Window* window, TTF_Font* font) {
    int rootData, numRootChildren;

    // Prompt user for the root data
    SDL_Event event;
    SDL_Color textColor = {255, 255, 255};
    SDL_Surface* textSurface = NULL;
    SDL_Texture* textTexture = NULL;

    SDL_Rect inputRect = {200, 300, 0, 0};
    char inputText[100] = "";
    char combinedText[200];

    while (1) {
        SDL_PollEvent(&event);

        // Handle SDL events for input
        if (event.type == SDL_QUIT) {
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
            return NULL;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_RETURN) {
                // Break the loop if Enter is pressed
                break;
            } else if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0) {
                // Handle backspace
                inputText[strlen(inputText) - 1] = '\0';
            } else if (event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) {
                // Handle Ctrl+C to copy (you can add more functionality if needed)
                SDL_SetClipboardText(inputText);
            } else if (event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
                // Handle Ctrl+V to paste (you can add more functionality if needed)
                strncpy(inputText, SDL_GetClipboardText(), sizeof(inputText) - 1);
            }
        } else if (event.type == SDL_TEXTINPUT) {
            // Handle text input
            strcat(inputText, event.text.text);
        }

        // Render text to the SDL window
        SDL_RenderClear(renderer);
        SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, background);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
        SDL_DestroyTexture(backgroundTexture);

        // Combine prompt and user input text
        sprintf(combinedText, "%s %s", "Enter the value for the root of the tree:", inputText);

        // Render combined text
        textSurface = TTF_RenderText_Solid(font, combinedText, textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        // Render combined text
        SDL_Rect inputTextRect = {inputRect.x, inputRect.y + inputRect.h, 0, 0};
        SDL_QueryTexture(textTexture, NULL, NULL, &inputTextRect.w, &inputTextRect.h);
        SDL_RenderCopy(renderer, textTexture, NULL, &inputTextRect);

        SDL_RenderPresent(renderer);

        // Cleanup
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

    }

    // Convert user input to integer
    rootData = atoi(inputText);

    // Prompt user for the number of children
    inputRect.y += inputRect.h + 10;
    inputText[0] = '\0';

    while (1) {
        SDL_PollEvent(&event);

        // Handle SDL events for input
        if (event.type == SDL_QUIT) {
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
            return NULL;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_RETURN) {
                // Break the loop if Enter is pressed
                break;
            } else if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0) {
                // Handle backspace
                inputText[strlen(inputText) - 1] = '\0';
            } else if (event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) {
                // Handle Ctrl+C to copy (you can add more functionality if needed)
                SDL_SetClipboardText(inputText);
            } else if (event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
                // Handle Ctrl+V to paste (you can add more functionality if needed)
                strncpy(inputText, SDL_GetClipboardText(), sizeof(inputText) - 1);
            }
        } else if (event.type == SDL_TEXTINPUT) {
            // Handle text input
            strcat(inputText, event.text.text);
        }

        // Render text to the SDL window
        SDL_RenderClear(renderer);
        

            SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, background);
            SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
            SDL_DestroyTexture(backgroundTexture);
        // Combine prompt and user input text
        sprintf(combinedText, "%s %s", "Enter the number of children for the root (0 to terminate):", inputText);

        // Render combined text
        textSurface = TTF_RenderText_Solid(font, combinedText, textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        // Render combined text
        SDL_Rect inputTextRect = {inputRect.x, inputRect.y + inputRect.h, 0, 0};
        SDL_QueryTexture(textTexture, NULL, NULL, &inputTextRect.w, &inputTextRect.h);
        SDL_RenderCopy(renderer, textTexture, NULL, &inputTextRect);

        SDL_RenderPresent(renderer);

        // Cleanup
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    // Convert user input to integer
    numRootChildren = atoi(inputText);

    Node* root = createNode(rootData, numRootChildren);

    // Pass the renderer and window to the function to handle rendering
    createChildrenSDL(root, renderer, window, font);

    return root;
}



int Delete(SDL_Renderer* renderer, SDL_Window* window, TTF_Font* font) {
    int delete;

    // Prompt user for the root data
    SDL_Event event;
    SDL_Color textColor = {255, 255, 255};
    SDL_Surface* textSurface = NULL;
    SDL_Texture* textTexture = NULL;

    SDL_Rect inputRect = {200, 300, 0, 0};
    char inputText[100] = "";
    char combinedText[200];

    while (1) {
        SDL_PollEvent(&event);

        // Handle SDL events for input
        if (event.type == SDL_QUIT) {
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_RETURN) {
                // Break the loop if Enter is pressed
                break;
            } else if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0) {
                // Handle backspace
                inputText[strlen(inputText) - 1] = '\0';
            } else if (event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) {
                // Handle Ctrl+C to copy (you can add more functionality if needed)
                SDL_SetClipboardText(inputText);
            } else if (event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
                // Handle Ctrl+V to paste (you can add more functionality if needed)
                strncpy(inputText, SDL_GetClipboardText(), sizeof(inputText) - 1);
            }
        } else if (event.type == SDL_TEXTINPUT) {
            // Handle text input
            strcat(inputText, event.text.text);
        }

        // Render text to the SDL window
        SDL_RenderClear(renderer);

        SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, background);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
        SDL_DestroyTexture(backgroundTexture);
        // Combine prompt and user input text
        sprintf(combinedText, "%s %s", "Enter the value you want destroy:", inputText);

        // Render combined text
        textSurface = TTF_RenderText_Solid(font, combinedText, textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        // Render combined text
        SDL_Rect inputTextRect = {inputRect.x, inputRect.y + inputRect.h, 0, 0};
        SDL_QueryTexture(textTexture, NULL, NULL, &inputTextRect.w, &inputTextRect.h);
        SDL_RenderCopy(renderer, textTexture, NULL, &inputTextRect);

        SDL_RenderPresent(renderer);

        // Cleanup
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

    }

    // Convert user input to integer
    delete= atoi(inputText);

    // Prompt user for the number of children
    inputRect.y += inputRect.h + 10;
    inputText[0] = '\0';

    return delete;
}


int Recherche(SDL_Renderer* renderer, SDL_Window* window, TTF_Font* font) {
    int Chercher;

    // Prompt user for the root data
    SDL_Event event;
    SDL_Color textColor = {255, 255, 255};
    SDL_Surface* textSurface = NULL;
    SDL_Texture* textTexture = NULL;

    SDL_Rect inputRect = {200, 300, 0, 0};
    char inputText[100] = "";
    char combinedText[200];

    while (1) {
        SDL_PollEvent(&event);

        // Handle SDL events for input
        if (event.type == SDL_QUIT) {
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_RETURN) {
                // Break the loop if Enter is pressed
                break;
            } else if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0) {
                // Handle backspace
                inputText[strlen(inputText) - 1] = '\0';
            } else if (event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) {
                // Handle Ctrl+C to copy (you can add more functionality if needed)
                SDL_SetClipboardText(inputText);
            } else if (event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
                // Handle Ctrl+V to paste (you can add more functionality if needed)
                strncpy(inputText, SDL_GetClipboardText(), sizeof(inputText) - 1);
            }
        } else if (event.type == SDL_TEXTINPUT) {
            // Handle text input
            strcat(inputText, event.text.text);
        }

        // Render text to the SDL window
        SDL_RenderClear(renderer);

        SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, background);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
        SDL_DestroyTexture(backgroundTexture);
        // Combine prompt and user input text
        sprintf(combinedText, "%s %s", "Enter the value you want To search:", inputText);

        // Render combined text
        textSurface = TTF_RenderText_Solid(font, combinedText, textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        // Render combined text
        SDL_Rect inputTextRect = {inputRect.x, inputRect.y + inputRect.h, 0, 0};
        SDL_QueryTexture(textTexture, NULL, NULL, &inputTextRect.w, &inputTextRect.h);
        SDL_RenderCopy(renderer, textTexture, NULL, &inputTextRect);

        SDL_RenderPresent(renderer);

        // Cleanup
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

    }

    // Convert user input to integer
    Chercher= atoi(inputText);

    // Prompt user for the number of children
    inputRect.y += inputRect.h + 10;
    inputText[0] = '\0';

    return Chercher;
}



//Recursively draws the tree
void drawTree(SDL_Renderer* renderer, Node* root, int x, int y, int level, int offsetX, int numberToHighlight, int Highlight) {
    if (root == NULL) {
        return;
    }

    // Draw the current node
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, x, y, x, y + 40);
    SDL_RenderDrawLine(renderer, x - 20, y + 40, x + 20, y + 40);
    char buffer[20];
    sprintf(buffer, "%d", root->data);
    
    // Use SDL_ttf to display the text
    SDL_Color textColor;
    if (Highlight == 1) {
        textColor = (SDL_Color){255, 255, 255, 255};  // Set color for text
        if (root->data == numberToHighlight) {
            textColor = (SDL_Color){255, 255, 0, 255}; // Change color for the highlighted number
        }
    } else {
        textColor = (SDL_Color){255, 255, 255, 255};
    }

    // Use SDL_ttf to display the text
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, buffer, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {x - 10, y + 40, 20, 20};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // Draw connections to children
    int childCount = root->childCount;
    if (childCount > 0) {
        int childSpacing = offsetX;
        int childX = x - (childSpacing * (childCount - 1) / 2);

        for (int i = 0; i < childCount; i++) {
            int childY = y + 80;
            SDL_RenderDrawLine(renderer, x, y + 60, childX, childY);
            drawTree(renderer, root->children[i], childX, childY, level + 1, offsetX / 2 + 50, numberToHighlight, Highlight);
            childX += childSpacing;
        }
    }
}





void initSDL() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
}

void createWindowAndRenderer() {
    window = SDL_CreateWindow("Arbre N-aires", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                              WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void loadFont() {
    font = TTF_OpenFont("fonts/LEMONMILK-Bold.otf", FONT_SIZE);
}

void renderText(const char* text, TTF_Font* font, SDL_Rect* rect, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_RenderCopy(renderer, texture, NULL, rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}



int main(int argc, char* argv[]) {
    initSDL();
    createWindowAndRenderer();
    ScreenSurface=SDL_GetWindowSurface(window);
    background=IMG_Load("images/blackboard-slate.jpg");
    loadFont();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT,2,2048);
    click=Mix_LoadWAV("sounds/interface-124464.mp3");
    hover=Mix_LoadWAV("sounds/Rollover_or_button_sound.wav");
    music = Mix_LoadMUS("sounds/Sadness and Sorrow.mp3");
    Mix_VolumeChunk(click,100);
   
    Mix_PlayMusic(music,-1);

    MenuItem menuItems[4];
    const char* options[] = {"Creation", "Supression", "Recherche", "exit"};


    SDL_Rect titleRect = {WINDOW_WIDTH / 2 - TITLE_FONT_SIZE * 6, 100, TITLE_FONT_SIZE * 12, TITLE_FONT_SIZE};

  

    for (int i = 0; i < 4; ++i) {
        menuItems[i].text = options[i];
        menuItems[i].rect.x = WINDOW_WIDTH / 2 - FONT_SIZE;
        menuItems[i].rect.y = 300 + i * FONT_SIZE * 2;
        menuItems[i].rect.w = FONT_SIZE * strlen(options[i]);
        menuItems[i].rect.h = FONT_SIZE;
        menuItems[i].color = (SDL_Color){255, 255, 255, 255};
        menuItems[i].hoverFlag = 0;   // White initially
    }

    int isRunning = 1;
    SDL_Event event;

    int currentMode = 2; // Initial mode is the menu

    SDL_Rect returnToMenuRect = {WINDOW_WIDTH / 2 - FONT_SIZE, WINDOW_HEIGHT - 100, FONT_SIZE * 7, FONT_SIZE};
    Node* root;
   
    int numberToHighlight,Highlight=0;
    int targetToDelete ;
    while (isRunning) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                if (currentMode == 2) {
                    if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &menuItems[0].rect)) {
                        Mix_PlayChannel(-1, click, 0);
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        SDL_RenderClear(renderer);
                        SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, background);
                        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
                        SDL_DestroyTexture(backgroundTexture);
                        root = createSampleTree(renderer, window, font);
                        currentMode = 1;
                        printf("--Your Tree is ready !");
                    } else if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &menuItems[1].rect)) {
                        Mix_PlayChannel(-1, click, 0);
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        SDL_RenderClear(renderer);
                        root = createSampleTree(renderer, window, font);
                        currentMode = 1;
                        targetToDelete=Delete(renderer, window, font);
                        printf("--Your Tree! is ready !(click to see change)"); // Demander à l'utilisateur d'entrer la valeur
                    } else if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &menuItems[2].rect)) {
                        Mix_PlayChannel(-1, click, 0);
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        SDL_RenderClear(renderer);
                        root = createSampleTree(renderer, window, font);
                        currentMode = 1;
                        Highlight=1;
                        numberToHighlight=Recherche(renderer, window, font);
                        printf("--Your Tree is ready !");
                    }else if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &menuItems[3].rect)) {
                        Mix_PlayChannel(-1, click, 0);
                        exit(0);
                        }
                } else if (currentMode == 1) {
                    if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &returnToMenuRect)) {
                        Mix_PlayChannel(-1, click, 0);
                        currentMode = 2;
                        break;
                    }
                    if (targetToDelete != -1) {
                        deleteNode(root, targetToDelete);
                        targetToDelete = -1;
                    }
                }
            }
        }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render the background image outside the if condition to make it visible in both menu and tree views
    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, background);
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    SDL_DestroyTexture(backgroundTexture);

    if (currentMode == 2) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

int hoverState[4] = {0}; // Initialize all states to 0 (not hovering)

for (int i = 0; i < 4; ++i) {
    if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &menuItems[i].rect)) {
        if (menuItems[i].hoverFlag == 0) {  // Check the hover flag
            Mix_PlayChannel(-1, hover, 0);
            menuItems[i].hoverFlag = 1;  // Set hover flag to 1 (hovered)
        }
        menuItems[i].color = (SDL_Color){255, 255, 0, 255};
    } else {
        menuItems[i].hoverFlag = 0;  // Reset hover flag if not hovering
        menuItems[i].color = (SDL_Color){255, 255, 255, 255};
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // Set color for the title text
    renderText(TITLE_TEXT, font, &titleRect, (SDL_Color){255, 255, 255, 255});
    SDL_Color textColor = menuItems[i].color;
    renderText(menuItems[i].text, font, &menuItems[i].rect, textColor);
}



    } else if (currentMode == 1) {
        int rootX = WINDOW_WIDTH / 2;
        int rootY = 50;
        int level = 0;
        int offsetX = 200;

        drawTree(renderer, root, rootX, rootY, level, offsetX, numberToHighlight, Highlight);

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        SDL_Color color_r;
        const char* text_r = "Return to Menu";

        if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &returnToMenuRect)) {
            color_r = (SDL_Color){255, 255, 0, 255};
        } else {
            color_r = (SDL_Color){255, 255, 255, 255};
        }

        renderText(text_r, font, &returnToMenuRect, color_r);
    }

    SDL_RenderPresent(renderer);
}    


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}





//cd C:\Users\MSI LAPTOP\Documents\test
//gcc src/menu.c -o prog1 -I include -L lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image
//prog1.exe
