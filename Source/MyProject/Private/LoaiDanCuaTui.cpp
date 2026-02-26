#include "LoaiDanCuaTui.h"

#include "GameFramework/Character.h"
#include "Materials/MaterialInstanceDynamic.h"

ALoaiDanCuaTui::ALoaiDanCuaTui()
{
    PrimaryActorTick.bCanEverTick = true;

    // Tạo SphereComponent và set làm Root
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere")); // SphereCollision da tinh toan va cham roi
    RootComponent = CollisionSphere;
    CollisionSphere->SetSphereRadius(8);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionSphere->SetCollisionObjectType(ECC_PhysicsBody);

    // Tạo StaticMesh và gắn vào Root
    ThayMeshVienDanVaoDay = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    ThayMeshVienDanVaoDay->SetupAttachment(CollisionSphere);
    ThayMeshVienDanVaoDay->SetCollisionEnabled(ECollisionEnabled::NoCollision); // them dong nay vao de Engine khong can tinh toan va cham cua mesh nua

    // Tạo ProjectileMovement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = CollisionSphere;
    ProjectileMovement->InitialSpeed = 8000.0f;
    ProjectileMovement->MaxSpeed = 8000.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.5f; // 0 = không bị ảnh hưởng gravity
    
    // ✅ Bind delegate tại đây
    CollisionSphere->OnComponentHit.AddDynamic(this, &ALoaiDanCuaTui::OnProjectileHit);
    
}

void ALoaiDanCuaTui::BeginPlay()
{
    Super::BeginPlay();
    
    // Lấy Owner (Người bắn)
    AActor* MyOwner = GetOwner();
    if (MyOwner)
    {
        // Lệnh này bảo Component vật lý của đạn lờ Owner đi
        CollisionSphere->IgnoreActorWhenMoving(MyOwner, true);
    }
}


void ALoaiDanCuaTui::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // Logic: Tranh vien dan ban ra va cham voi nguoi choi hoac khau sung vi cham vao Owner
    // Tranh 2 vien dan vua ban ra va cham va dinh vao nhau loi logic physics// 1. Kiểm tra va chạm cơ bản
    // if (!OtherActor || OtherActor == this || OtherActor == OwnerActor || OtherActor->IsA(ALoaiDanCuaTui::StaticClass()))
    if (OtherActor && OtherActor != this && OtherActor != OwnerActor && OtherComp)
    {
        // UMeshComponent* SkeletalMesh = OtherActor->FindComponentByClass<UMeshComponent>();
        // UStaticMeshComponent* StaticMesh = OtherActor->FindComponentByClass<UStaticMeshComponent>();
        
        // 1. Kiểm tra xem thứ vừa chạm vào có phải là Skeletal Mesh (Enemy) không
        USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(OtherComp);
        
        // 2. Kiểm tra xem có phải là Static Mesh (Vật cản) không
        UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(OtherComp);
        
        if (SkeletalMesh)
        {
            if (SkeletalMesh && SkeletalMesh->GetNumMaterials() > 0)
            {
                // 2. Tạo DIM và ép màu đỏ
                UMaterialInstanceDynamic* DIMEnemy = SkeletalMesh->CreateDynamicMaterialInstance(0);
                {
                    DIMEnemy->SetVectorParameterValue(TEXT("Paint Tint"), FLinearColor::Red);
                    // UE_LOG(LogTemp, Warning, TEXT("Hit! Doi mau do thanh cong"));
                }

                // 3. Set Timer để tự phục hồi Material cho kẻ địch
                // Chú ý: Ta truyền thẳng EnemyOriginalMat và SkeletalMesh vào Lambda
                GetWorld()->GetTimerManager().SetTimer(
                    ResetTimerHandle,
                    [DIMEnemy]() 
                    {
                        if (DIMEnemy)
                        {
                            DIMEnemy->SetVectorParameterValue(TEXT("Paint Tint"), FLinearColor::White);
                        }
                    },
                    ThoiGianDoiMau, false);
            }
        }
        
        else if (StaticMesh)
        {
            // Kiểm tra xem Mesh có Material nào không trước khi truy cập index 0
            if (StaticMesh->GetNumMaterials() > 0)
            {
                UMaterialInstanceDynamic* DIMStatic = StaticMesh->CreateDynamicMaterialInstance(0);
                // if (!bIsMauGocCuaStaticSaved)
                // {
                //     DIMStatic->GetVectorParameterValue(FName("Base Color"), MauGocCuaStatic);
                //     bIsMauGocCuaStaticSaved = true;
                //     UE_LOG(LogTemp, Log, TEXT("Da luu mau goc: %s"), *MauGocCuaStatic.ToString());
                // }
                
                if (DIMStatic->Parent)
                {
                    DIMStatic->Parent->GetVectorParameterValue(FName("Base Color"), MauGocCuaStatic);
                }
                else 
                {
                    // Phòng hờ nếu không có Parent, thì mới lấy màu hiện tại
                    DIMStatic->GetVectorParameterValue(FName("Base Color"), MauGocCuaStatic);
                }
                
                // Thực hiện logic đổi màu:
                DIMStatic->SetVectorParameterValue(TEXT("Base Color"), FLinearColor::Blue);
                UE_LOG(LogTemp, Warning, TEXT("Hit! Doi mau xanh thanh cong"));
                
                if (DIMStatic)
                {
                    GetWorld()->GetTimerManager().ClearTimer(ResetTimerHandle);
                    GetWorld()->GetTimerManager().SetTimer(
                        ResetTimerHandle,
                        [this, DIMStatic]() // Capture OriginalColor vào lambda
                        {
                            if (DIMStatic)
                            {
                                DIMStatic->SetVectorParameterValue(FName("Base Color"),MauGocCuaStatic);
                            }
                        },
                        ThoiGianDoiMau, false);
                }
            }
        }
        
        // 4. Thay vì Destroy, hãy Ẩn viên đạn và tắt va chạm để nó giống như đã biến mất
        CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        ThayMeshVienDanVaoDay->SetVisibility(false);

        // Bây giờ bạn có thể Destroy nó an toàn sau khi thời gian đổi màu kết thúc 
        // (Cộng thêm 0.1s để chắc chắn timer đổi màu chạy xong)
        GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, 
            this, &ALoaiDanCuaTui::SelfDestroy, 
            ThoiGianDoiMau + 0.1f, false);
    }
    
    // Destroy bullet after short delay
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ALoaiDanCuaTui::SelfDestroy, 2.0f, false);
    }
}


void ALoaiDanCuaTui::SelfDestroy()
{
    Destroy();
}


void ALoaiDanCuaTui::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}


