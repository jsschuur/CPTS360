#include "Card.h"
#include <deque>
#include <vector>
#include <algorithm>
#include <iostream>
class Deck
{
private:
	std::deque<Card*> deck;
	std::vector<Card*> discard;
public:
	std::vector<Card*> *getDiscardPtr() { return &discard; }

	void shuffle() { std::random_shuffle(deck.begin(), deck.end()); }
	int numCard() { return deck.size(); }
	Card *draw()
	{
		Card *temp = deck.front();
		deck.pop_front();
		discard.push_back(temp);
		return temp;
	}

	void loadDeck(int numDecks)
	{

		SDL_Texture *back = TextureManager::LoadTexture("deck/cardback1.png");

		SDL_Texture *club2 = TextureManager::LoadTexture("deck/2club.png");
		SDL_Texture *diam2 = TextureManager::LoadTexture("deck/2diam.png");
		SDL_Texture *heart2 = TextureManager::LoadTexture("deck/2heart.png");
		SDL_Texture *spad2 = TextureManager::LoadTexture("deck/2spad.png");

		SDL_Texture *club3 = TextureManager::LoadTexture("deck/3club.png");
		SDL_Texture *diam3 = TextureManager::LoadTexture("deck/3diam.png");
		SDL_Texture *heart3 = TextureManager::LoadTexture("deck/3heart.png");
		SDL_Texture *spad3 = TextureManager::LoadTexture("deck/3spad.png");

		SDL_Texture *club4 = TextureManager::LoadTexture("deck/4club.png");
		SDL_Texture *diam4 = TextureManager::LoadTexture("deck/4diam.png");
		SDL_Texture *heart4 = TextureManager::LoadTexture("deck/4heart.png");
		SDL_Texture *spad4 = TextureManager::LoadTexture("deck/4spad.png");

		SDL_Texture *club5 = TextureManager::LoadTexture("deck/5club.png");
		SDL_Texture *diam5 = TextureManager::LoadTexture("deck/5diam.png");
		SDL_Texture *heart5 = TextureManager::LoadTexture("deck/5heart.png");
		SDL_Texture *spad5 = TextureManager::LoadTexture("deck/5spad.png");

		SDL_Texture *club6 = TextureManager::LoadTexture("deck/6club.png");
		SDL_Texture *diam6 = TextureManager::LoadTexture("deck/6diam.png");
		SDL_Texture *heart6 = TextureManager::LoadTexture("deck/6heart.png");
		SDL_Texture *spad6 = TextureManager::LoadTexture("deck/6spad.png");

		SDL_Texture *club7 = TextureManager::LoadTexture("deck/7club.png");
		SDL_Texture *diam7 = TextureManager::LoadTexture("deck/7diam.png");
		SDL_Texture *heart7 = TextureManager::LoadTexture("deck/7heart.png");
		SDL_Texture *spad7 = TextureManager::LoadTexture("deck/7spad.png");

		SDL_Texture *club8 = TextureManager::LoadTexture("deck/8club.png");
		SDL_Texture *diam8 = TextureManager::LoadTexture("deck/8diam.png");
		SDL_Texture *heart8 = TextureManager::LoadTexture("deck/8heart.png");
		SDL_Texture *spad8 = TextureManager::LoadTexture("deck/8spad.png");

		SDL_Texture *club9 = TextureManager::LoadTexture("deck/9club.png");
		SDL_Texture *diam9 = TextureManager::LoadTexture("deck/9diam.png");
		SDL_Texture *heart9 = TextureManager::LoadTexture("deck/9heart.png");
		SDL_Texture *spad9 = TextureManager::LoadTexture("deck/9spad.png");

		SDL_Texture *club10 = TextureManager::LoadTexture("deck/10club.png");
		SDL_Texture *diam10 = TextureManager::LoadTexture("deck/10diam.png");
		SDL_Texture *heart10 = TextureManager::LoadTexture("deck/10heart.png");
		SDL_Texture *spad10 = TextureManager::LoadTexture("deck/10spad.png");

		SDL_Texture *clubjack = TextureManager::LoadTexture("deck/jackclub.png");
		SDL_Texture *diamjack = TextureManager::LoadTexture("deck/jackdiam.png");
		SDL_Texture *heartjack = TextureManager::LoadTexture("deck/jackheart.png");
		SDL_Texture *spadjack = TextureManager::LoadTexture("deck/jackspad.png");

		SDL_Texture *clubqueen = TextureManager::LoadTexture("deck/queenclub.png");
		SDL_Texture *diamqueen = TextureManager::LoadTexture("deck/queendiam.png");
		SDL_Texture *heartqueen = TextureManager::LoadTexture("deck/queenheart.png");
		SDL_Texture *spadqueen = TextureManager::LoadTexture("deck/queenspad.png");

		SDL_Texture *clubking = TextureManager::LoadTexture("deck/kingclub.png");
		SDL_Texture *diamking = TextureManager::LoadTexture("deck/kingdiam.png");
		SDL_Texture *heartking = TextureManager::LoadTexture("deck/kingheart.png");
		SDL_Texture *spadking = TextureManager::LoadTexture("deck/kingspad.png");

		SDL_Texture *clubace = TextureManager::LoadTexture("deck/aceclub.png");
		SDL_Texture *diamace = TextureManager::LoadTexture("deck/acediam.png");
		SDL_Texture *heartace = TextureManager::LoadTexture("deck/aceheart.png");
		SDL_Texture *spadace = TextureManager::LoadTexture("deck/acespad.png");

		for (int i = 0; i < numDecks; i++) {

			deck.push_back(new Card(club6, 6, back));
			deck.push_back(new Card(club6, 6, back));
			deck.push_back(new Card(clubqueen, 10, back));
			deck.push_back(new Card(club6, 6, back));
			deck.push_back(new Card(club6, 6, back));

			deck.push_back(new Card(club2, 2, back));
			deck.push_back(new Card(diam2, 2, back));
			deck.push_back(new Card(heart2, 2, back));
			deck.push_back(new Card(spad2, 2, back));

			deck.push_back(new Card(club3, 3, back));
			deck.push_back(new Card(diam3, 3, back));
			deck.push_back(new Card(heart3, 3, back));
			deck.push_back(new Card(spad3, 3, back));

			deck.push_back(new Card(club4, 4, back));
			deck.push_back(new Card(diam4, 4, back));
			deck.push_back(new Card(heart4, 4, back));
			deck.push_back(new Card(spad4, 4, back));

			deck.push_back(new Card(club5, 5, back));
			deck.push_back(new Card(diam5, 5, back));
			deck.push_back(new Card(heart5, 5, back));
			deck.push_back(new Card(spad5, 5, back));

			deck.push_back(new Card(club6, 6, back));
			deck.push_back(new Card(diam6, 6, back));
			deck.push_back(new Card(heart6, 6, back));
			deck.push_back(new Card(spad6, 6, back));

			deck.push_back(new Card(club7, 7, back));
			deck.push_back(new Card(diam7, 7, back));
			deck.push_back(new Card(heart7, 7, back));
			deck.push_back(new Card(spad7, 7, back));

			deck.push_back(new Card(club8, 8, back));
			deck.push_back(new Card(diam8, 8, back));
			deck.push_back(new Card(heart8, 8, back));
			deck.push_back(new Card(spad8, 8, back));

			deck.push_back(new Card(club9, 9, back));
			deck.push_back(new Card(diam9, 9, back));
			deck.push_back(new Card(heart9, 9, back));
			deck.push_back(new Card(spad9, 9, back));

			deck.push_back(new Card(club10, 10, back));
			deck.push_back(new Card(diam10, 10, back));
			deck.push_back(new Card(heart10, 10, back));
			deck.push_back(new Card(spad10, 10, back));

			deck.push_back(new Card(clubjack, 10, back));
			deck.push_back(new Card(diamjack, 10, back));
			deck.push_back(new Card(heartjack, 10, back));
			deck.push_back(new Card(spadjack, 10, back));

			deck.push_back(new Card(clubqueen, 10, back));
			deck.push_back(new Card(diamqueen, 10, back));
			deck.push_back(new Card(heartqueen, 10, back));
			deck.push_back(new Card(spadqueen, 10, back));

			deck.push_back(new Card(clubking, 10, back));
			deck.push_back(new Card(diamking, 10, back));
			deck.push_back(new Card(heartking, 10, back));
			deck.push_back(new Card(spadking, 10, back));

			deck.push_back(new Card(clubace, 11, back, true));
			deck.push_back(new Card(diamace, 11, back, true));
			deck.push_back(new Card(heartace, 11, back, true));
			deck.push_back(new Card(spadace, 11, back, true));
		}
	}

	void clean() { 
		for (auto &c : deck) c->clean(); 
		for (auto &c : discard) c->clean();
	}

	void reshuffle(int numDecks)
	{
		for (auto &c : discard) c->clean();
		for (auto &c : deck) c->clean();

		loadDeck(numDecks);
	}
};