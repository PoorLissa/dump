#include "stdafx.h"
#include "Game.h"

// ���������� ������, ����� ������ � ���� ��������� �� ���������� ����� �������� � ����� � ����� �������� �������� ��� � ���� ����� 1 ���
std::string strMsg;

// ���������� ������ - ������ ���� ����� ���� ��� �������� ��������� ���� � ��������
gameCells GameCells;
// ------------------------------------------------------------------------------------------------------------------------



Game::Game()
{
    m_BulletShader     = 0;
	m_Bitmap_Bgr	   = 0;
    sprIns1            = 0;
    sprIns2            = 0;
    sprIns3            = 0;
    sprIns4            = 0;
    m_PlayerBitmapIns1 = 0;
    m_PlayerBitmapIns2 = 0;
    m_BulletBitmapIns  = 0;
    thPool             = 0;
}
// ------------------------------------------------------------------------------------------------------------------------



Game::~Game()
{
}
// ------------------------------------------------------------------------------------------------------------------------



bool Game::Init(int screenWidth, int screenHeight, HighPrecisionTimer *appTimer, GraphicsClass *m_Graph, HWND hwnd)
{
    bool result;

    m_Graphics    = m_Graph;
	scrWidth      = screenWidth;
	scrHeight     = screenHeight;
    scrHalfWidth  = screenWidth  / 2;
    scrHalfHeight = screenHeight / 2;
    bgrWidth      = screenWidth  * 1.5;
    bgrHeight     = screenHeight * 1.5;
    wndPosX       = 0;
    wndPosY       = 0;
    borderZone    = scrWidth / 4;

    // ������� ������. ���� ��� ������������ ������ ��� ������� ���������� ������� ��� ��������, onTimer �� ������������
    gameTimer.Initialize(100);

    // ��� ������� (� ����� ������� �������)
    thPool = new ThreadPool(50);
    gameObjectBase::setThreadPool(thPool);

    // �������������� ����� �����
    GameCells.Init(bgrWidth, bgrHeight);

    srand(unsigned int(time(0)));

    // -------------------------------------------------

    // �������, ����������� ��� ����
    SAFE_CREATE(m_BulletShader, bulletShader_Instancing);

	// Initialize the texture shader objects:
    if( !m_BulletShader->Initialize(m_Graphics->m_d3d->GetDevice(), hwnd) ) 
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}



	// --- Bitmaps and Sprites ---
	{
		// Create and initialize the bitmap objects:
        SAFE_CREATE(m_Bitmap_Bgr, BitmapClass);

        result = m_Bitmap_Bgr->Initialize(m_Graphics->m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/Bgr.png", bgrWidth, bgrHeight);
        CHECK_RESULT(hwnd, result, L"Could not initialize the bitmap object.");

        // ���������������� ������� � ���������� ��������
        // ����� ������������ �� ��� � �������� ��������� ������, ��� � � ���������� �������
        // � ����� ������, � ��������� ������ ���������� ������ �������
        sprIns1 = new InstancedSprite(scrWidth, scrHeight);
            if( !sprIns1 ) return false;
        sprIns2 = new InstancedSprite(scrWidth, scrHeight);
            if( !sprIns2 ) return false;
        sprIns3 = new InstancedSprite(scrWidth, scrHeight);
            if( !sprIns3 ) return false;
        sprIns4 = new InstancedSprite(scrWidth, scrHeight);
            if( !sprIns4 ) return false;

        m_PlayerBitmapIns1 = new InstancedSprite(scrWidth, scrHeight);
            if( !m_PlayerBitmapIns1 ) return false;
        m_PlayerBitmapIns2 = new InstancedSprite(scrWidth, scrHeight);
            if( !m_PlayerBitmapIns2 ) return false;
        m_BulletBitmapIns  = new InstancedSprite(scrWidth, scrHeight);
            if( !m_BulletBitmapIns  ) return false;
    }

    // �������, ������ 1
    {
        // ���������� �����, 10 ������ 200x310
        WCHAR *frames[] = { L"../DirectX-11-Tutorial/data/walkingdead.png" };

        result = sprIns1->Initialize(m_Graphics->m_d3d->GetDevice(), screenWidth, screenHeight, frames, 1, 45, 45, 200, 310);
        CHECK_RESULT(hwnd, result, L"Could not initialize the instanced sprite object.");

        monsterList1.spriteInst    = sprIns1;   // ������������ ���������������� ������
        monsterList1.rotationAngle = -90.0f;    // ����, �� ������� ����� ��������� ������ ������
        monsterList1.framesQty     = 9;         // ����� ������ �������� � ���������������� ������� (������� ������ �� ���������� ����� ������ � ��������)

        for (unsigned int i = 0; i < MONSTERS_QTY; i++) {
            int        x = 50 + rand() % (bgrWidth  - 100);
            int        y = 50 + rand() % (bgrHeight - 100);
            float  speed = (rand() % 250 + 10) * 0.1f;
			float  scale = 0.5f + (rand() % 16) * 0.1f;
            int interval = int(50 / speed);

            //speed *= 0.1f;
            //speed = 1.0f;
            //speed = 0.0f;

            monsterList1.pushBack(x, y, scale, speed, interval);
        }
    }

    // �������, ������ 2
    {
        // ������ �������, 8 ����
		WCHAR *frames[] = {	L"../DirectX-11-Tutorial/data/monster1/001.png",
							L"../DirectX-11-Tutorial/data/monster1/002.png",
							L"../DirectX-11-Tutorial/data/monster1/003.png",
							L"../DirectX-11-Tutorial/data/monster1/004.png",
							L"../DirectX-11-Tutorial/data/monster1/005.png",
							L"../DirectX-11-Tutorial/data/monster1/006.png",
							L"../DirectX-11-Tutorial/data/monster1/007.png",
							L"../DirectX-11-Tutorial/data/monster1/008.png"
		};

        unsigned int framesNum = sizeof(frames) / sizeof(frames[0]);

        result = sprIns2->Initialize(m_Graphics->m_d3d->GetDevice(), screenWidth, screenHeight, frames, framesNum, 30, 30);
        CHECK_RESULT(hwnd, result, L"Could not initialize the instanced sprite object.");

        monsterList2.spriteInst    = sprIns2;   // ������������ ���������������� ������
        monsterList2.rotationAngle = 0.0f;      // ����, �� ������� ����� ��������� ������ ������
        monsterList2.framesQty     = 8;         // ����� ������ �������� � ���������������� ������� (������� ������ �� ���������� ����� ������ � ��������)

        for (int i = 0; i < MONSTERS_QTY; i++) {
            int        x = 50 + rand() % (bgrWidth  - 100);
            int        y = 50 + rand() % (bgrHeight - 100);
            float  speed = (rand() % 250 + 10) * 0.1f;
			float  scale = 0.5f + (rand() % 16) * 0.1f;
            int interval = int(50 / speed);

            //speed *= 0.1f;
            //speed = 1.0f;
            //speed = 0.0f;

            monsterList2.pushBack(x, y, scale, speed, interval);
        }
    }



    // ����� ������ ������� � ���������
    VEC.push_back(&monsterList1);
    VEC.push_back(&monsterList2);



    // ������
    {
        // ������ �������, 1 ��
        // ������� ���������� ������ ������ ��������� � �������� ������� � BonusEffects::Effects!
		WCHAR *frames[] = {	L"../DirectX-11-Tutorial/data/bonus_Heal.png",
                            L"../DirectX-11-Tutorial/data/bonus_Freeze.png",
                            L"../DirectX-11-Tutorial/data/bonus_Shield.png",
                            L"../DirectX-11-Tutorial/data/bonus_Piercing.png",
                            L"../DirectX-11-Tutorial/data/bonus_Slow.png"
        };

        unsigned int framesNum = sizeof(frames) / sizeof(frames[0]);

        result = sprIns3->Initialize(m_Graphics->m_d3d->GetDevice(), screenWidth, screenHeight, frames, framesNum, 45, 45);
        CHECK_RESULT(hwnd, result, L"Could not initialize the instanced sprite object for Bonuses.");

        bonusList1.spriteInst    = sprIns3;
        bonusList1.rotationAngle = 0.0f;

        for (int i = 0; i < framesNum; i++) {
            int       x = 50 + rand() % (scrWidth  - 100) + 300;
            int       y = 50 + rand() % (scrHeight - 100);

            bonusList1.objList.push_back(new Bonus(x, y, Bonus::Effects(i)));
            bonusList1.listSize++;
        }
    }



    // ������
    {
        // ������ �������, 3 ����
		WCHAR *frames[] = {	L"../DirectX-11-Tutorial/data/bonus_weapon_Pistol.png",
							L"../DirectX-11-Tutorial/data/bonus_weapon_Rifle.png",
							L"../DirectX-11-Tutorial/data/bonus_weapon_Shotgun.png",
                            L"../DirectX-11-Tutorial/data/bonus_weapon_Ion_gun1.png",
                            L"../DirectX-11-Tutorial/data/bonus_weapon_Plasma_gun.png"
		};

        unsigned int framesNum = sizeof(frames) / sizeof(frames[0]);

        result = sprIns4->Initialize(m_Graphics->m_d3d->GetDevice(), screenWidth, screenHeight, frames, framesNum, 30, 30);
        CHECK_RESULT(hwnd, result, L"Could not initialize the instanced sprite object.");

        weaponList1.spriteInst = sprIns4;
        weaponList1.listSize   = 0;


        for (int i = BonusWeapons::Weapons::PISTOL; i < BonusWeapons::Weapons::_lastWeapon; i++) {
            int x = 50 + rand() % (scrWidth  - 100);
            int y = 50 + rand() % (scrHeight - 100);

            weaponList1.objList.push_back(new Weapon(x, y, BonusWeapons::Weapons(i)));
            weaponList1.listSize++;
        }
    }



    // �����
    {
		WCHAR *frames1[] = { L"../DirectX-11-Tutorial/data/tank_body_1.png" };
        result = m_PlayerBitmapIns1->Initialize(m_Graphics->m_d3d->GetDevice(), screenWidth, screenHeight, frames1, 1, 40, 40);
        CHECK_RESULT(hwnd, result, L"Could not initialize the instanced sprite object for the Player.");

		WCHAR *frames2[] = { L"../DirectX-11-Tutorial/data/tank_tower_1.png" };
        result = m_PlayerBitmapIns2->Initialize(m_Graphics->m_d3d->GetDevice(), screenWidth, screenHeight, frames2, 1, 40, 40);
        CHECK_RESULT(hwnd, result, L"Could not initialize the instanced sprite object for the Player.");

		m_Player = new Player(scrHalfWidth, scrHalfHeight, screenWidth/800, 90.0f, 5.0f, 1000, 1, appTimer);
        ((Player*)m_Player)->resetBulletsType();
        ((Player*)m_Player)->setEffect(Weapon::BonusWeapons::PISTOL);

//        ((Player*)m_Player)->setBulletsType_On(Player::BulletsType::PIERCING);
//        ((Player*)m_Player)->setBulletsType_On(Player::BulletsType::ION);
    }



    // ����
    {
        Bullet::setScrSize(bgrWidth, bgrHeight);
        //WCHAR *frames2[] = { L"../DirectX-11-Tutorial/data/bullet-red-icon-128.png" };
        WCHAR *frames2[] = { L"../DirectX-11-Tutorial/data/bullet-red-icon-128_blurred.png" };
        result = m_BulletBitmapIns->Initialize(m_Graphics->m_d3d->GetDevice(), screenWidth, screenHeight, frames2, 1, 10, 10);
        CHECK_RESULT(hwnd, result, L"Could not initialize the instanced sprite object for the bullet.");
        // ??? ���� �� ����� ���� �� ���� �������� �� ���� ����, ��� �������� ��� ������
        //bulletList.objList.push_back(new Bullet(-100, -100, 1.0f, -105, -105, 1.0, 0));
        bulletList.objList.push_back(new Bullet(-5, -5, 1.0f, -6, -6, 1.0, 0));
        bulletList.listSize++;
    }



	// ������
	{
        SAFE_CREATE(m_Cursor, BitmapClass);
		result = m_Cursor->Initialize(m_Graphics->m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/cursor.png", 24, 24);
        CHECK_RESULT(hwnd, result, L"Could not initialize the cursor object.");
	}

    return true;
}
// ------------------------------------------------------------------------------------------------------------------------



void Game::Shutdown()
{
    // �����
    if (m_Player)
        SAFE_DELETE(m_Player);

    // ������ ����
    if( bulletList.listSize > 0 ) {
        auto iter = bulletList.objList.begin(),
              end = bulletList.objList.end();
        while (iter != end) {
            SAFE_DELETE(*iter);
            ++iter;
        }
    }

    // ������ ������� ��������
    if( VEC.size() ) {
    
        for (unsigned int i = 0; i < VEC.size(); i++) {

            std::list<gameObjectBase*> *list = &(VEC[i]->objList);

            if( list && list->size() ) {
            
                auto iter = list->begin(),
                      end = list->end();
                while (iter != end) {
                    SAFE_DELETE(*iter);
                    ++iter;
                }
            }
        }
    }

    SAFE_SHUTDOWN(m_PlayerBitmapIns1);
    SAFE_SHUTDOWN(m_PlayerBitmapIns2);
    SAFE_SHUTDOWN(m_BulletBitmapIns);
    SAFE_SHUTDOWN(sprIns1);
    SAFE_SHUTDOWN(sprIns2);
    SAFE_SHUTDOWN(sprIns3);
    SAFE_SHUTDOWN(sprIns4);

	// Release the Bitmap objects:
    SAFE_SHUTDOWN(m_Bitmap_Bgr);
    SAFE_SHUTDOWN(m_Cursor);

    // Release the Shader objects:
    SAFE_SHUTDOWN(m_BulletShader);

    SAFE_DELETE(thPool);
}
// ------------------------------------------------------------------------------------------------------------------------



// ������������ � �������� ������� �������
bool Game::Render2d(const float &rotation, const float &zoom, const int &mouseX, const int &mouseY, const keysPressed *Keys, const bool & onTimer)
{
    static ID3D11Device        *m_Device     = m_Graphics->m_d3d->GetDevice();
    static ID3D11DeviceContext *m_devContext = m_Graphics->m_d3d->GetDeviceContext();

    static BonusParams bParam;

    // ??? ����� �� �������� ��� � ������ ��� �������?..

//    float ZoomFactor = 0.66f;

	// ���
	{
		// �������� ����� � �����
		if (!m_Bitmap_Bgr->Render(m_devContext, 0, 0) )
			return false;

		// ������������ ����������� �������������� ������
		D3DXMatrixTranslation(&m_Graphics->matrixTranslation, 0.0f + wndPosX, 0.0f - wndPosY, 0.0f);
		//D3DXMatrixScaling(&matrixScaling, 1.0f + 0.01f*sin(rotation/10) + 0.0001f*zoom, 1.0f + 0.01f*sin(rotation/10) + 0.0001f*zoom, 1.0f);

		// Once the vertex / index buffers are prepared we draw them using the texture shader.
		// Notice we send in the matrixOrthographic instead of the projectionMatrix for rendering 2D.
		// Do note also that if your view matrix is changing you will need to create a default one for 2D rendering and use it instead of the regular view matrix.
		// In this tutorial using the regular view matrix is fine as the camera in this tutorial is stationary.

		// �������� ������ ��� ������ ����������� �������
		if ( !m_Graphics->m_TextureShader->Render(m_devContext, m_Bitmap_Bgr->GetIndexCount(),
				m_Graphics->matrixWorldZ * m_Graphics->matrixTranslation * m_Graphics->matrixScaling,
					m_Graphics->matrixView, m_Graphics->matrixOrthographic, m_Bitmap_Bgr->GetTexture()) )
                        return false;
	}

//    D3DXMatrixScaling(&m_Graphics->matrixScaling, ZoomFactor, ZoomFactor, 1.0f);

	// ����������� ������ - ������ ��� �������!
	{
		if (!m_Graphics->m_Bitmap_Tree->Render(m_devContext, scrHalfWidth - 128, scrHalfHeight - 128))
			return false;

		D3DXMatrixRotationZ(&m_Graphics->matrixWorldZ, rotation / 5);
		D3DXMatrixTranslation(&m_Graphics->matrixTranslation, 100.0f, 100.0f, 0.0f);
		D3DXMatrixScaling(&m_Graphics->matrixScaling, 0.85f + 0.03f*sin(rotation) + 0.0001f*zoom, 0.85f + 0.03f*sin(rotation) + 0.0001f*zoom, 1.0f);

		if ( !m_Graphics->m_TextureShader->Render(m_devContext, m_Graphics->m_Bitmap_Tree->GetIndexCount(),
				m_Graphics->matrixWorldZ * m_Graphics->matrixTranslation * m_Graphics->matrixScaling,
					m_Graphics->matrixView, m_Graphics->matrixOrthographic, m_Graphics->m_Bitmap_Tree->GetTexture()) )
		                return false;
	}



    // render sprites from vector using Instancing
    {
        // reset world matrices
        m_Graphics->m_d3d->GetWorldMatrix(m_Graphics->matrixWorldZ);
        m_Graphics->m_d3d->GetWorldMatrix(m_Graphics->matrixTranslation);
        m_Graphics->m_d3d->GetWorldMatrix(m_Graphics->matrixScaling);

        static float playerPosX = 0, playerPosY = 0;

        // �� ����� ������������� � ���������� �� GPU ������� ������ � ������ ������, ����� ��� �������������� � ������������� � ��������, ��� ������� ��� FPS
        if( onTimer )
        {
            static float timeAdd = appTimerInterval * 0.001;

            // ����������� ����� ������� ���������� ������� (� ��������)
            int t1 = int(timeElapsed);
            timeElapsed  += timeAdd;                            
            int t2 = int(timeElapsed);

            // ������������, ������� �������� ����� ��������� �� ���� ��������
            if( t2 > t1 )
                qtyToGenerate = log10(int(timeElapsed));
            else
                qtyToGenerate = 0;

            {
#if defined doLogMessages
                gameTimer.Frame();

                strMsg += "time passed: ";
                strMsg += std::to_string(gameTimer.GetTime());
                strMsg += "\n";
#endif
            }

            std::list<gameObjectBase*>::iterator iter, end;

            Player *player = (Player*)m_Player;

            // �������� ��������������� ��������� ��� �������� ���������� � ����� Move() ��� ������� Bonus � Weapon
            if( weaponList1.listSize > 0 || bonusList1.listSize > 0 ) {

                bParam.player = player;
                bParam.mouseX = const_cast<int*>(&mouseX);
                bParam.mouseY = const_cast<int*>(&mouseY);
            }

            // --- Player ---
            {
                player->setDirectionL( Keys->left  );
                player->setDirectionR( Keys->right );
                player->setDirectionU( Keys->up    );
                player->setDirectionD( Keys->down  );

                player->Move();

                // ������� ������� ���������� ������
                playerPosX = m_Player->getPosX();
                playerPosY = m_Player->getPosY();

#if 1
                // ���������� ����� ���� ������������ ����� ����
                {
                    if( playerPosX > scrWidth - borderZone ) {

                        if( bgrWidth - scrWidth + wndPosX > 1 ) {
                            // ���� ����� �������� ����� ������, ������� ���, � ����� �������� �� ���� ��������� ����
                            wndPosX += scrWidth - borderZone - playerPosX;
                            playerPosX = scrWidth - borderZone;
                        }
                        else {
                            // ���� ����� ��� ������� ������, ������������ ��� �� �����, � ������ ��������� ����� �� ���� ������
                            wndPosX = scrWidth - bgrWidth;
                            if( playerPosX > scrWidth - 25 ) {
                                playerPosX = scrWidth - 25;
                            }
                        }
                    }

                    if( playerPosX < borderZone ) {

                        if( wndPosX < 0 ) {
                            // ���� ����� �������� ����� �����, ������� ���, � ����� �������� �� ���� ����
                            wndPosX += borderZone - playerPosX;
                            playerPosX = borderZone;
                        }
                        else {
                            // ���� ����� ��� ������� ������, ������������ ��� �� �����, � ������ ��������� ����� �� ���� ������
                            wndPosX = 0;
                            if( playerPosX < 25 )
                                playerPosX = 25;
                        }
                    }

                    if( playerPosY > scrHeight - borderZone ) {

                        if( bgrHeight - scrHeight + wndPosY > 1 ) {
                            // ���� ����� �������� ����� ����, ������� ���, � ����� �������� �� ���� ����
                            wndPosY += scrHeight - borderZone - playerPosY;
                            playerPosY = scrHeight - borderZone;

                        }
                        else {
                            // ���� ����� ��� ������� ������, ������������ ��� �� �����, � ������ ��������� ����� �� ���� ������
                            wndPosY = scrHeight - bgrHeight;
                            if( playerPosY > scrHeight - 25 )
                                playerPosY = scrHeight - 25;
                        }
                    }

                    if( playerPosY < borderZone ) {

                        if( wndPosY < 0 ) {
                            // ���� ����� �������� ����� �����, ������� ���, � ����� �������� �� ���� ����
                            wndPosY += borderZone - playerPosY;
                            playerPosY = borderZone;
                        }
                        else {
                            // ���� ����� ��� ������� ������, ������������ ��� �� �����, � ������ ��������� ����� �� ���� ������
                            wndPosY = 0;
                            if( playerPosY < 25 )
                                playerPosY = 25;
                        }
                    }

                    m_Player->setPosX(playerPosX);
                    m_Player->setPosY(playerPosY);
                }
#endif


                if (!m_PlayerBitmapIns1->initializeInstances(m_Device, m_Player))
                    return false;
                if (!m_PlayerBitmapIns2->initializeInstances(m_Device, m_Player))
                    return false;
            }

            // --- Bullets ---
            {
                #define BulletObj (*iter)

                // ���� ������ ����� ������ ����, ��������� � ������� ����� ���� � ������������ � ������� ����� ������ � ������� ���������
                if ( Keys->lmbDown ) {

                    if( player->isWeaponReady() )
                        player->spawnBullet_Normal(mouseX, mouseY, &bulletList.objList, bulletList.listSize, wndPosX, wndPosY);

                }

                // ������������ �������� ���� ����
                iter = bulletList.objList.begin();
                end  = bulletList.objList.end();

                while (iter != end) {

                    if( BulletObj->isAlive() ) {

                        // ??? - ��������� �������� ������� ������ � ������ � ���� �� ������, �� ��� ����������� ������ ����� ������� ����, ����� ���� �������������
                        // ??? - � ����� ���������� � ��������, �������, ��� �������� ���� ���� �����
                        BulletObj->Move(wndPosX, wndPosY);
                    }
                    else {

                        delete BulletObj;
                        iter = bulletList.objList.erase(iter);
                        bulletList.listSize--;
                        continue;
                    }

                    ++iter;
                }
                thPool->waitForAll();   // ����, ���� ��� ������ ���������� �� �����

                if (!m_BulletBitmapIns->initializeInstances(m_Device, &bulletList.objList, &bulletList.listSize, true))
                    return false;

                {
#if defined doLogMessages
                    gameTimer.Frame();

                    strMsg += "bullets time: ";

                    strMsg += std::to_string(gameTimer.GetTime());
                    strMsg += "; bullets num: ";
                    strMsg += std::to_string(bulletList.listSize);
                    strMsg += "\n";
#endif
                }
            }

            // --- Monsters with threads ---
            {
                // ��������� ��������� ������� ������� � ��������� � ��������� ������
                // ��� ���, � ��������, ��� ������ ����� ��������� �������� (� �������� ���������), ��� ������� ��� ����� ��������������
                for (unsigned int listNo = 0; listNo < VEC.size(); listNo++)
                    thPool->runAsync(&Game::threadMonsterMove, this, listNo, playerPosX, playerPosY);

                thPool->waitForAll();   // ����, ���� ��� ������ ���������� �� �����

                {
#if defined doLogMessages
                    gameTimer.Frame();

                    strMsg += "-=-=-=-=-=- >>> Monsters time:  ";
                    strMsg += std::to_string(gameTimer.GetTime());
                    strMsg += "\n";
#endif
                }
            }

            // --- Bonuses ---
            {
                #define BonusObj (*iter)

                if( bonusList1.listSize ) {

                    iter = bonusList1.objList.begin();
                    end  = bonusList1.objList.end();

                    while (iter != end) {

                        if( BonusObj->isAlive() ) {

                            BonusObj->Move(wndPosX, wndPosY, &bParam);

                        }
                        else {

                            delete BonusObj;
                            iter = bonusList1.objList.erase(iter);
                            bonusList1.listSize--;
                            continue;
                        }

                        ++iter;
                    }

                    if( !bonusList1.spriteInst->initializeInstances(m_Device, &bonusList1.objList, &bonusList1.listSize) )
                        return false;
                }
            }

            // Weapons
            {
                #define WeaponObj (*iter)
                if( weaponList1.listSize ) {

                    iter = weaponList1.objList.begin();
                    end  = weaponList1.objList.end();

                    while (iter != end) {

                        if( WeaponObj->isAlive() ) {

                            WeaponObj->Move(wndPosX, wndPosY, &bParam);

                        }
                        else {

                            delete WeaponObj;
                            iter = weaponList1.objList.erase(iter);
                            weaponList1.listSize--;
                            continue;
                        }

                        ++iter;
                    }

                    if( !weaponList1.spriteInst->initializeInstances(m_Device, &weaponList1.objList, &weaponList1.listSize) )
                        return false;
                }
            }

            // --- Generate new Monsters ---
            {
                UINT vecNo;
                int x, y;

qtyToGenerate = 0;

                for (int i = 0; i < qtyToGenerate; i++) {
                
                    vecNo = rand() % VEC.size();

                    if( rand()%2 ) {
                        x = rand()%(bgrWidth+200) - 100;
                        y = rand()%2 ? -100 : bgrHeight + 100;
                    }
                    else {
                        y = rand()%(bgrHeight+200) - 100;
                        x = rand()%2 ? -100 : bgrWidth + 100;
                    }

                    float  speed = (rand() % 250 + 10) * 0.1f;
			        float  scale = 0.5f + (rand() % 16) * 0.1f;
                    int interval = int(50 / speed);

                    VEC[vecNo]->pushBack(x, y, scale, speed, interval);
                }
            }

        } // if-onTimer



        // --- ���������� ������ �������� � GPU � ����� ������������ ��� ��������� ---
        // ---------------------------------------------------------------------------

        D3DXMatrixTranslation(&m_Graphics->matrixTranslation, wndPosX, -wndPosY, 1.0f);
//D3DXMatrixScaling(&m_Graphics->matrixScaling, ZoomFactor, ZoomFactor, 1.0f);

        // Monsters from all the lists
        {
            InstancedSprite *Sprite;
            for (unsigned int listNo = 0; listNo < VEC.size(); listNo++) {

                if( (VEC)[listNo]->listSize > 0 ) {

                    Sprite = VEC[listNo]->spriteInst;

                    if( !Sprite->Render(m_devContext) )
                        return false;

                    // Apply Matrices if needed
#if 0
                    D3DXMatrixRotationZ(&matrixWorldZ, rotation / 5);
                    D3DXMatrixTranslation(&matrixTranslation, 100.0f, 100.0f, 0.0f);
                    D3DXMatrixScaling(&matrixScaling, 0.5f + 0.3*sin(rotation/5) + 0.0001*zoom, 0.5f + 0.3*sin(rotation/5) + 0.0001*zoom, 1.0f);
                    D3DXMatrixScaling(&matrixScaling, 1.0f + 0.1*sin(rotation) + 0.1*zoom, 1.0f + 0.1*sin(rotation) + 0.1*zoom, 1.0f);
#endif

                    // Render the sprites using the texture shader
                    if( !m_Graphics->m_TextureShaderIns->Render(m_devContext,
                            Sprite->GetVertexCount(), Sprite->GetInstanceCount(),
                                m_Graphics->matrixWorldZ * m_Graphics->matrixTranslation * m_Graphics->matrixScaling,
                                    m_Graphics->matrixView, m_Graphics->matrixOrthographic,
                                        Sprite->GetTextureArray(), 1, playerPosX - scrHalfWidth - wndPosX, scrHalfHeight - playerPosY + wndPosY) )
                    return false;
                }
            }
        }

        m_Graphics->m_d3d->GetWorldMatrix(m_Graphics->matrixTranslation);

        // Player
        {
            // ������
            if( !m_PlayerBitmapIns1->Render(m_devContext) )
                return false;

            if( !m_Graphics->m_TextureShaderIns->Render(m_devContext,
                    m_PlayerBitmapIns1->GetVertexCount(), m_PlayerBitmapIns1->GetInstanceCount(),
                        m_Graphics->matrixWorldZ * m_Graphics->matrixTranslation * m_Graphics->matrixScaling,
                            m_Graphics->matrixView, m_Graphics->matrixOrthographic,
                                m_PlayerBitmapIns1->GetTextureArray(), 0, 0, 0) )
            return false;

            // �����
            if( !m_PlayerBitmapIns2->Render(m_devContext) )
                return false;

            if( !m_Graphics->m_TextureShaderIns->Render(m_devContext,
                    m_PlayerBitmapIns2->GetVertexCount(), m_PlayerBitmapIns2->GetInstanceCount(),
                        m_Graphics->matrixWorldZ * m_Graphics->matrixTranslation * m_Graphics->matrixScaling,
                            m_Graphics->matrixView, m_Graphics->matrixOrthographic,
                                m_PlayerBitmapIns2->GetTextureArray(), 1, mouseX - scrHalfWidth, scrHalfHeight - mouseY) )
            return false;
        }

        // �������� ��� ������� ������� � ������������ �� ������� ����
        D3DXMatrixTranslation(&m_Graphics->matrixTranslation, wndPosX, -wndPosY, 1.0f);

        // Bullets
        if( bulletList.listSize > 0 )
        {
            if( !m_BulletBitmapIns->Render(m_devContext) )
                return false;

            if( !m_BulletShader->Render(m_devContext,
                    m_BulletBitmapIns->GetVertexCount(), m_BulletBitmapIns->GetInstanceCount(),
                        m_Graphics->matrixWorldZ * m_Graphics->matrixTranslation * m_Graphics->matrixScaling,
                            m_Graphics->matrixView, m_Graphics->matrixOrthographic, m_BulletBitmapIns->GetTextureArray(), 0, 0, 0) )
            return false;
        }
        _itoa_s(bulletList.listSize, chBuffer, 100, 10);
        msg = chBuffer;

        // Bonuses
        if( bonusList1.listSize > 0 )
        {
            if( !bonusList1.spriteInst->Render(m_devContext) )
                return false;

            if( !m_Graphics->m_TextureShaderIns->Render(m_devContext,
                    bonusList1.spriteInst->GetVertexCount(), bonusList1.spriteInst->GetInstanceCount(),
                        m_Graphics->matrixWorldZ * m_Graphics->matrixTranslation * m_Graphics->matrixScaling,
                            m_Graphics->matrixView, m_Graphics->matrixOrthographic, bonusList1.spriteInst->GetTextureArray(), 0, 0, 0) )
            return false;        
        }

        // Weapons
        if( weaponList1.listSize > 0 )
        {
            if( !sprIns4->Render(m_devContext) )
                return false;

            if( !m_Graphics->m_TextureShaderIns->Render(m_devContext,
                    weaponList1.spriteInst->GetVertexCount(), weaponList1.spriteInst->GetInstanceCount(),
                        m_Graphics->matrixWorldZ * m_Graphics->matrixTranslation * m_Graphics->matrixScaling,
                            m_Graphics->matrixView, m_Graphics->matrixOrthographic, weaponList1.spriteInst->GetTextureArray(), 0, 0, 0) )
            return false;        
        }

	    // �������� ������
	    {
		    if (!m_Cursor->Render(m_devContext, mouseX, mouseY))
			    return false;

		    m_Graphics->m_d3d->GetWorldMatrix(m_Graphics->matrixWorldX);
		    if( !m_Graphics->m_TextureShader->Render(m_devContext, m_Cursor->GetIndexCount(),
				    m_Graphics->matrixWorldX, m_Graphics->matrixView, m_Graphics->matrixOrthographic, m_Cursor->GetTexture()) )
		    return false;
	    }
    }
}
// ------------------------------------------------------------------------------------------------------------------------



// ��������� ����������� ��������
void Game::threadMonsterMove(const unsigned int &listNo, const unsigned int &playerPosX, const unsigned int &playerPosY)
{
    #define MonsterObj (*iter)
    MonsterList *monsterList = VEC[listNo];

    auto iter = monsterList->objList.begin();
    auto end  = monsterList->objList.end();

    while (iter != end) {

        if( MonsterObj->isAlive() ) {

            MonsterObj->Move(playerPosX - wndPosX, playerPosY - wndPosY);

        }
        else {

            // ??? do we need to lock this part?
            delete MonsterObj;
            iter = monsterList->objList.erase(iter);
            monsterList->listSize--;

            continue;
        }

        // ���� �� ����� ������ ��������, ������ ������� �� ����� ������� �� ������� (������ �����, �.�. ����� ������ �� m_textures->Release())
        #if defined DO_HIDE_MONSTERS
                MonsterObj->setAlive(false);
        #endif

        ++iter;
    }

    static ID3D11Device *m_Device = m_Graphics->m_d3d->GetDevice();

    if( !monsterList->spriteInst->initializeInstances(m_Device, &monsterList->objList, &monsterList->listSize) )
        return;

    #undef MonsterObj

    return;
}
// ------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------