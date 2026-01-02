// main.cc
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "G4PhysListFactory.hh"
#include "G4EmStandardPhysics_option4.hh" // 가장 정밀한 EM 모델 (EMZ)
#include "G4EmParameters.hh"             // MSC 파라미터 제어용

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

int main(int argc, char** argv)
{
    // 1. RunManager 생성 (Serial 모드 권장 - 디버깅 용이)
    auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Serial);

    // 2. 기하 구조 초기화 (Pb | Scintillator Interface)
    runManager->SetUserInitialization(new DetectorConstruction());

    // 3. 물리 리스트 초기화
    // 기본 베이스: FTFP_BERT (고에너지 표준)
    G4PhysListFactory physFactory;
    G4VModularPhysicsList* physicsList = physFactory.GetReferencePhysList("FTFP_BERT");
    
    // [중요] EM Physics 교체: Standard -> Option4
    // Option4는 저에너지 도달 입자 및 경계면 처리에 가장 적합한 정밀 모델입니다.
    physicsList->ReplacePhysics(new G4EmStandardPhysics_option4());
    runManager->SetUserInitialization(physicsList);

    // ========================================================================
    // [KEY FEATURE] Boundary Crossing Artifact Control Center
    // ========================================================================
    // 이 섹션에서 MSC(다중산란) 파라미터를 조정하여 아티팩트를 재현하거나 해결합니다.
    G4EmParameters* emParams = G4EmParameters::Instance();

    // ------------------------------------------------------------------------
    // [Scenario 1: Case A - Reproduction of Artifacts (Default)]
    // 주석을 해제하면 'Dip'과 'V-Shape Truncation'이 발생합니다.
    // ------------------------------------------------------------------------
    // emParams->SetMscSkin(0);            // 경계면 정밀 추적 비활성화
    // emParams->SetMscRangeFactor(0.04);  // 스텝 제한 완화 (큰 스텝 허용)
    // G4cout << ">>> [CONFIG] Case A: Default (Artifact Expected)" << G4endl;

    // ------------------------------------------------------------------------
    // [Scenario 2: Case B - Physical Correction (Active)]
    // 현재 활성화된 상태. 'Dip'이 사라지고 물리적 분포가 복원됩니다.
    // ------------------------------------------------------------------------
    emParams->SetMscSkin(3);            // 경계면 3 lambda 이내에서 Single Scattering 강제
    emParams->SetMscRangeFactor(0.02);  // 스텝 크기를 비정의 2%로 엄격히 제한
    G4cout << ">>> [CONFIG] Case B: Corrected (Physics Restored)" << G4endl;
    // ========================================================================

    // 4. 사용자 액션 (Primary Generator, Stepping Action 등)
    runManager->SetUserInitialization(new ActionInitialization());

    // 5. 초기화
    runManager->Initialize();

    // 6. UI 및 실행
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    G4VisManager* visManager = nullptr;

    if (argc == 1) {
        // 인터랙티브 모드 (GUI 실행)
        visManager = new G4VisExecutive;
        visManager->Initialize();
        G4UIExecutive* ui = new G4UIExecutive(argc, argv);
        
        // 초기 시각화 설정 (선택 사항)
        // UImanager->ApplyCommand("/control/execute vis.mac"); 
        
        ui->SessionStart();
        delete ui;
        delete visManager;
    } else {
        // 배치 모드 (매크로 실행)
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command + fileName);
    }

    delete runManager;
    return 0;
}
