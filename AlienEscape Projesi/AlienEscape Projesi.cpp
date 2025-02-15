
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace sf;

void drawSimpleSadEmoji(RenderWindow& window, float x, float y) { //gameover da mutsuz emojisi
	// Sol göz
	RectangleShape leftEye(Vector2f(10, 20)); 
	leftEye.setFillColor(Color::Black);
	leftEye.setPosition(x + 20, y + 20);

	// Sağ göz
	RectangleShape rightEye(Vector2f(10, 20));
	rightEye.setFillColor(Color::Black);
	rightEye.setPosition(x + 60, y + 20);

	
	RectangleShape mouthLeft(Vector2f(20, 5));
	mouthLeft.setFillColor(Color::Black);
	mouthLeft.setPosition(x + 30, y + 60);
	mouthLeft.setRotation(30); 

	RectangleShape mouthMiddle(Vector2f(20, 5));
	mouthMiddle.setFillColor(Color::Black);
	mouthMiddle.setPosition(x + 40, y + 70);

	RectangleShape mouthRight(Vector2f(20, 5));
	mouthRight.setFillColor(Color::Black);
	mouthRight.setPosition(x + 50, y + 60);
	mouthRight.setRotation(-30);

	window.draw(leftEye);
	window.draw(rightEye);
	window.draw(mouthLeft);
	window.draw(mouthMiddle);
	window.draw(mouthRight);
}

class Alien { //uzaylının random hareketi ve boyutlarını ayarlayan alien classı
public:
	Sprite sprite; 	
	vector<Texture> textures;
	int currentframe; //çerçeve dizisi
	Vector2f velocity; //uzaylının hızı;
	Clock animationclock; //uzaylı animasyonunu temsil eden clock.
	int outOfBoundsCount;
	bool isOutOfBoundsProcessed = false;

	Alien(vector<Texture>& textures, float speed) : textures(textures), currentframe(0), outOfBoundsCount(0), isOutOfBoundsProcessed(false) { //uzaylı sınıfının kurucu fonksiyonu
		sprite.setTexture(textures[0]);
		spawn(speed);
	}

		void spawn(float speed) {
			float startX, startY;
			do {
				startX = rand() % 800;
				startY = rand() % 600;
			} while (pow(startX - 400, 2) + pow(startY - 500, 2) < 900);
			sprite.setPosition(startX, startY);

			do {
				velocity = Vector2f((rand() % 3 - 1) * speed, (rand() % 3 - 1) * speed);
			} while (velocity.x == 0 && velocity.y == 0);
		}

	
	void move(float alienspeed) { //karakterin hareketini sağlar ayrıca ters yönde veya pozitif yönde yönlendirir.(parametreyi alienspeed olarak alır.)
		sprite.move(velocity);


		if (velocity.x > 0) {
			sprite.setScale(0.2f, 0.2f); //sağ kısım
		}
		else if (velocity.x < 0) {
			sprite.setScale(-0.2f, 0.2f); //sol kısım
		}

		if (isOutOfBounds()) {
			if (!isOutOfBoundsProcessed) {
				outOfBoundsCount++;
				isOutOfBoundsProcessed = true;

				float newX = (rand() % 2 == 0) ? -sprite.getGlobalBounds().width : 800 + sprite.getGlobalBounds().width;
				float newY = (rand() % 2 == 0) ? -sprite.getGlobalBounds().height : 600 + sprite.getGlobalBounds().height;
				sprite.setPosition(newX, newY);

				do {
					spawn(alienspeed);
				} while (isOutOfBounds() || (velocity.x == 0 && velocity.y == 0) );

			}
		}
		else {
				isOutOfBoundsProcessed = false;
		}

		//uzaylı spritelerini animasyonize eder.
		if (animationclock.getElapsedTime().asMilliseconds() > 100) { //animationclock milisn cinsinden (kare hızı) 100 den büyükse bu koşula girer(karakterin efektif bi şekilde hareket etmesini sağlar.)
			if (!textures.empty()) {
				currentframe = (currentframe + 1) % textures.size(); //textures size göre current frame günceller
				sprite.setTexture(textures[currentframe]);
			}
			animationclock.restart(); //restart edip clock u döngüye sokar.
		}	
	}
		
	bool isOutOfBounds() { //uzaylıyı sınır dışına çıkarmak için bool kontrolü
		return (sprite.getPosition().x + sprite.getGlobalBounds().width < -50 ||
				sprite.getPosition().x > 800 + 50||
				sprite.getPosition().y + sprite.getGlobalBounds().height < -50 ||
				sprite.getPosition().y > 600 + 50);
	}
};

class Player { //Oyuncunun sağ sol aşağı yukarı hareketini sağlayan oyuncu(player) sınıfı
public:
	Sprite sprite;
	vector<Texture> textures;
	int currentframe;
	Clock animationclock;

	bool facingRight; //karakterin sol veya sağ tuşa basıldığında yön durumunu kontrol eder true= sağ ; false = sol


	Player(vector<Texture>& textures) : textures(textures) , currentframe(0) , facingRight(true) { //player kurucu fonk.
		sprite.setTexture(textures[0]);
		sprite.setScale(0.09f, 0.09f);
		sprite.setPosition(400, 500);

		sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
	}



	void handleInp() { //klavye girişlerini yöneten fonk.
		bool ismoving = false;
		Vector2f moveSpeed(0.0f, 0.0f); //karakter kontrolü için

		if (Keyboard::isKeyPressed(Keyboard::Left) && sprite.getPosition().x > 0) {
			moveSpeed.x = -5; //sol tuşa basıldığında 5 birim sola hareket eder oyuncu
			if (facingRight) {
				facingRight = false;
				sprite.setScale(-0.09f, 0.09f);
			}

			ismoving = true;

		}
		if (Keyboard::isKeyPressed(Keyboard::Right) && sprite.getPosition().x < 800 - sprite.getGlobalBounds().width) {
			moveSpeed.x = 5; //sağ tuşa basıldığında 5 birim sağa hareket eder
			if (!facingRight) {
				facingRight = true;
				sprite.setScale(0.09f, 0.09f);
			}
			
			ismoving = true;
			
		}
		if (Keyboard::isKeyPressed(Keyboard::Up) && sprite.getPosition().y > 0) {
			moveSpeed.y = -5; // yukarı tuşa basıldığında 5 birim yukarı hareket eder.
			ismoving = true;
		}
		if (Keyboard::isKeyPressed(Keyboard::Down) && sprite.getPosition().y < 600 - sprite.getGlobalBounds().height) {
			moveSpeed.y = 5; // aşağı tuşa basıldığında 5 birim aşağı hareket eder.
			ismoving = true;
		}

		sprite.move(moveSpeed);


		//ana karakter spritelerini animasyonize eder.
		if (ismoving && animationclock.getElapsedTime().asMilliseconds() > 100) {
			currentframe = (currentframe + 1) % textures.size();
			sprite.setTexture(textures[currentframe]);
			sprite.setScale(facingRight ? 0.09f : -0.09f , 0.09f);
			animationclock.restart();
		}
	}
};


class Game { // Temel işlemleri yapan ana oyun(GAME) sınıfı
private:
	RenderWindow window;

	vector<Texture> alientextures;
	vector<Texture> charactertextures;

	const vector<string> alienfilenames = {
	"assets/Uzaylı Karakter png/green__0000_idle_1.png",
	"assets/Uzaylı Karakter png/green__0001_idle_2.png",
	"assets/Uzaylı Karakter png/green__0002_idle_3.png",
	"assets/Uzaylı Karakter png/green__0003_turn_1.png",
	"assets/Uzaylı Karakter png/green__0004_turn_2.png",
	"assets/Uzaylı Karakter png/green__0005_turn_3.png",
	"assets/Uzaylı Karakter png/green__0006_walk_1.png",
	"assets/Uzaylı Karakter png/green__0007_walk_2.png",
	"assets/Uzaylı Karakter png/green__0008_walk_3.png",
	"assets/Uzaylı Karakter png/green__0009_walk_4.png",
	"assets/Uzaylı Karakter png/green__0010_walk_5.png",
	"assets/Uzaylı Karakter png/green__0011_walk_6.png"
	};

	const vector<string> characterfilenames = {
	"assets/Character png/standing_000.png",
	"assets/Character png/walk_000.png",
	"assets/Character png/walk_001.png",
	"assets/Character png/walk_002.png",
	"assets/Character png/walk_003.png",
	"assets/Character png/walk_004.png",
	"assets/Character png/walk_005.png"
	};
	vector<Sprite> aliensprites;
	vector<Sprite> charactersprites;

	Texture backgroundtextures;
	Sprite backgroundsprite;

	Player* player; //tek bir player yönetileceği için pointer olarak ekleyerek dby sağladım.
	vector<Alien> aliens;

	Font font;
	Text scoretext;
	int score;

	Clock clock; //skor sayması için
	float alienspeed;
	int aliencount; //5 uzaylı sonrası +1 uzaylı çıkması için sayaç
	int outOfBoundsCount;

	bool isgameactive = false; //ilk durum false olacaktır.
	bool isrulesactive = false;
	bool isgameover = false;
	bool isRestarting = false; //oyunun yeniden başlatıldığını ayırt etmek için

	void resetgame() {
		aliens.clear(); //uzaylı spritelerini temizler.
		score = 0; //oyun başına dönüleceği için skor sıfırlanır.
		alienspeed = 2.0f; //uzaylı hızı standart hale gelir.
		aliencount = 1;

		if (isRestarting) {
			isgameover = false;
			isgameactive = true;
		}
		else {
			isgameover = false;
			isgameactive = false;
		}

		Alien newAlien(alientextures, alienspeed);
		newAlien.sprite.setScale(0.2f, 0.2f);
		aliens.push_back(newAlien);
	}
	void loadtexturesandsprites() {

		if (!font.loadFromFile("assets/fonts/arial.ttf")) {
			cerr << "Failed to load default font" << endl;
			exit(-1);
		}
		for (const auto& FileName1 : alienfilenames) {
			Texture txture1;
			if (!txture1.loadFromFile(FileName1)) {
				cerr << "Failed to load " << FileName1 << endl;
				continue;
			}
			alientextures.push_back(txture1);
		}


		for (const auto& FileName2 : characterfilenames) {
			Texture txture2;
			if (!txture2.loadFromFile(FileName2)) {
				cerr << "Failed to load " << FileName2 << endl;
				continue;
			}
			charactertextures.push_back(txture2);
		}

		if (!backgroundtextures.loadFromFile("assets/spacealien.png")) {
			cerr << "Failed to load assets/spacealien.png" << endl;
			exit(-1);
		}

		backgroundsprite.setTexture(backgroundtextures);

	}

	void handleEvents() { //Events
		Event e; //e eventin(olayın) başlaması için bir nesnedir. pencerenin program boyunca açık kalmasını sağlar.
		while (window.pollEvent(e)) {
			if (e.type == e.Closed) {
				window.close();
			}
			if (!isgameactive && !isrulesactive && !isgameover && e.type == Event::MouseButtonPressed) {
				auto mousepos = Mouse::getPosition(window);
				if (mousepos.x >= 300 && mousepos.x <= 500 && mousepos.y >= 250 && mousepos.y <= 300) {
					isgameactive = true;
				}
				else if (mousepos.x >= 300 && mousepos.x <= 500 && mousepos.y >= 320 && mousepos.y <= 370) {
					isrulesactive = true;
				}

			}
			if (isrulesactive && e.type == Event::KeyPressed && e.key.code == Keyboard::Escape) {
				isrulesactive = false;
			}
			if (isgameover && e.type == Event::KeyPressed) {
				if(e.key.code == Keyboard::Enter) {
					isRestarting = true; //oyun tekrar başlar
					resetgame();
			}
				else if (e.key.code == Keyboard::Escape) {
					isRestarting = false; //ana menüye döner
					resetgame();
				}
			
			}
		}
	}

	void update() {
		if (isgameover) return; //base case hal

		player->handleInp(); //ana karakter kontrollerini sağlamak için handleınp fonk. erişiriz pointer üzerinden.

		const int maxAliens = 20; //uzaylı max sayısı sınırı
		const float maxSpeed = 6.5f; //uzaylı max hız sınırı

		for (auto& alien : aliens) {
			alien.move(alienspeed);

			if (alien.outOfBoundsCount >= 5) {
				alien.outOfBoundsCount = 0;

				if(aliens.size() < maxAliens) {
				if (!alientextures.empty()) {
					Alien newAlien(alientextures, alienspeed);
					newAlien.sprite.setScale(0.25f, 0.25f);
					aliens.push_back(newAlien);
				}
				}

				// Mevcut tüm uzaylıların hızını artır
				for (auto& alien : aliens) {
					alien.velocity.x = min(alien.velocity.x * 1.1f, maxSpeed);
					alien.velocity.y = min(alien.velocity.y * 1.1f, maxSpeed);
				}
				alienspeed += 0.5f;
				aliencount++;
			
					
			}

			if (alien.sprite.getGlobalBounds().intersects(player->sprite.getGlobalBounds())) {
				isgameover = true;
				isgameactive = false;
				return;
			}
		}

		if (clock.getElapsedTime().asSeconds() >= 1) { //geçen süreyi alıp her seferinde 2 artırır
			score += 2 * aliens.size(); //uzaylı sayısı kadar puan verir.
			clock.restart();
		}
	}

	void renderStartScreen() { //başlama ekranı
		window.clear(Color::White);

		if (!font.loadFromFile("assets/fonts/arial.ttf")) {
			cerr << "Failed to load default font." << endl;
			return;
		}

		Text title("Alien Escape", font, 50); //başlangıç penceresinin başlık kısmı
		title.setPosition(260,50);
		title.setFillColor(Color::Black);

		RectangleShape startbutton(Vector2f(200, 50)); //başlangıç penceresinin başlama butonu
		startbutton.setPosition(300, 250);
		startbutton.setFillColor(Color::Blue);

		Text starttext("Start Game", font, 25);
		starttext.setPosition(340, 260);
		starttext.setFillColor(Color::White);

		RectangleShape rulesbutton(Vector2f(200, 50));
		rulesbutton.setPosition(300, 320);
		rulesbutton.setFillColor(Color::Blue);

		Text rulestext("Rules", font, 25);
		rulestext.setPosition(370, 330);
		rulestext.setFillColor(Color::White);

		RectangleShape mainSquare(Vector2f(300, 300)); 
		mainSquare.setFillColor(Color::White);        
		mainSquare.setOutlineColor(Color::Black);    
		mainSquare.setOutlineThickness(5);            
		mainSquare.setPosition(250, 170);

		RectangleShape arrowBody1(Vector2f(100, 20));
		arrowBody1.setFillColor(Color::Black);
		arrowBody1.setPosition(600, 520);


		CircleShape arrowHead1(20, 3);
		arrowHead1.setFillColor(Color::Black);
		arrowHead1.setPosition(730, 510);
		arrowHead1.setRotation(90);

		window.draw(title);
		window.draw(mainSquare);
		window.draw(arrowBody1);
		window.draw(arrowHead1);
		window.draw(startbutton);
		window.draw(starttext);
		window.draw(rulesbutton);
		window.draw(rulestext);

		window.display();
	}

	void renderRulesScreen() {
		window.clear(Color::White);

		Text rulestitle("Game Rules", font, 45);
		rulestitle.setPosition(270, 30);
		rulestitle.setFillColor(Color::Black);

		RectangleShape rulesbutton(Vector2f(300, 80));
		rulesbutton.setPosition(255, 20);
		rulesbutton.setFillColor(Color::Blue);

		Text rulesBody("1. Öncelikle amaciniz gelen uzaylilardan kacmaktir\n"
					   "2. 5 uzayli kacis --> +1 uzayli. Oyun giderek zorlasir\n"
					   "3. 1sn hayatta kalma 2puan,oyun zorlastikca ise 4puan)\n"
					   "4. Uzayli yakalarsa --> GAME OVER! ve bastan baslar\n", font, 50);
		rulesBody.setCharacterSize(30);
		rulesBody.setPosition(50, 150);
		rulesBody.setFillColor(Color::Black);

		RectangleShape rulesBodyButton(Vector2f(750,10));
		rulesBodyButton.setPosition(30, 315);
		rulesBodyButton.setFillColor(Color::Black);

		RectangleShape rulesBodyButton1(Vector2f(750, 10));
		rulesBodyButton1.setPosition(30, 130);
		rulesBodyButton1.setFillColor(Color::Black);

		Text exitText("Press ESC to go back", font, 20);
		exitText.setPosition(300, 500);
		exitText.setFillColor(Color::Red);

		RectangleShape exitButton(Vector2f(200, 50));
		exitButton.setPosition(300, 490);
		exitButton.setFillColor(Color::Blue);

		RectangleShape arrowBody(Vector2f(100, 20));
		arrowBody.setFillColor(Color::Black);
		arrowBody.setPosition(120, 505);


		CircleShape arrowHead(20, 3); 
		arrowHead.setFillColor(Color::Black);
		arrowHead.setPosition(100, 535);
		arrowHead.setRotation(270);
	

		window.draw(rulesbutton);
		window.draw(rulestitle);
		window.draw(arrowBody);
		window.draw(arrowHead);
		window.draw(rulesBodyButton1);
		window.draw(rulesBodyButton);
		window.draw(rulesBody);
		window.draw(exitButton);
		window.draw(exitText);
		

		window.display();	
	}

	void renderGame() { //bu fonksiyon çizdirdiğim ve görüntülediğim (draw , display) tüm spriteleri başlatır.
		window.clear();
		window.draw(backgroundsprite);

		for (auto& alien : aliens) {
			window.draw(alien.sprite);
		}
		
		player->sprite.setScale(player->facingRight ? 0.09f : -0.09f, 0.09f);
		window.draw(player->sprite); //for döngüsü olarak ulaşmak yerine player spriteine pointerden ulaşıyorum.

		scoretext.setString("Score: " + to_string(score)); //skoru string olarak ayarlıyoruz.
		window.draw(scoretext);

		window.display();
	}
	void renderGameOverScreen() {
		window.clear(Color::White);

		Text gameoverText("Game Over!", font, 50);
		gameoverText.setPosition(280, 110);
		gameoverText.setFillColor(Color::Red);


		Text restartText("Press ENTER to restart", font, 25);
		restartText.setPosition(280, 280);
		restartText.setFillColor(Color::Red);

		RectangleShape restartbutton(Vector2f(260, 50));
		restartbutton.setPosition(280, 280);
		restartbutton.setFillColor(Color::Black);


		Text finalScoreText("Your Final Score: " + to_string(score), font, 25);
		finalScoreText.setPosition(305, 340);
		finalScoreText.setFillColor(Color::Blue);

		Text menuText("Press ESC to return to Main Menu", font, 20);
		menuText.setPosition(260, 400);
		menuText.setFillColor(Color::Red);

		RectangleShape menubutton(Vector2f(310, 50));
		menubutton.setPosition(260, 400);
		menubutton.setFillColor(Color::Black);


		RectangleShape gamesoverSquare(Vector2f(330, 330));
		gamesoverSquare.setFillColor(Color::White);
		gamesoverSquare.setOutlineColor(Color::Black);
		gamesoverSquare.setOutlineThickness(5);
		gamesoverSquare.setPosition(250, 170);



		window.draw(gameoverText);
		window.draw(gamesoverSquare);
		window.draw(restartbutton);
		window.draw(restartText);
		window.draw(finalScoreText);
		window.draw(menubutton);
		window.draw(menuText);

		drawSimpleSadEmoji(window, 370, 180);

		window.display();
	}

		

public:
	Game() : window(VideoMode(800, 600), "Welcome To AlienEscape Game", Style::Default) , score(0) , alienspeed(2.0f) , aliencount(1) , isgameover(false) , isrulesactive(false) , isgameactive(false) { //kurucu üye fonksiyonu
		window.setFramerateLimit(60);

		loadtexturesandsprites();
		
		player = new Player(charactertextures); //karakteri başlattık.
		player->sprite.setScale(0.09f, 0.09f);


		for (int i = 0; i < aliencount; i++) { //uzaylıları başlattık.
			Alien newAlien(alientextures, alienspeed);
			newAlien.sprite.setScale(0.2f, 0.2f);
			aliens.push_back(newAlien);
		}

		//skoretext kuruyoruz.	
		scoretext.setFont(font);
		scoretext.setCharacterSize(25);	
		scoretext.setFillColor(Color::White);
		scoretext.setPosition(10, 10);
	}

	~Game() { //yok edici fonk.
		delete player;
	}

	void run() {
		while (window.isOpen()) { //bu döngü programın çalıştığı süre boyunca açık kalmasını sağlar. Aksi takdirde aniden açılıp kapanacaktır.
			handleEvents();

			if (isgameactive) {
				update();
				renderGame();
			}
			else if (isrulesactive) {
				renderRulesScreen();
			}
			else if (isgameover) {
				renderGameOverScreen();
			}
			else {
				renderStartScreen();	
			}
			
		}
	}
};

int main() {
	srand(static_cast<unsigned>(time(nullptr))); //random için seed(unsigned olarak yapmamızın sebebi daha dinamik olması içinidr daha efektif çalışır ayrıca time fonk. unsigned int bekler.)

	Game game; //game nesnesi
	game.run();



	return 0;
}




