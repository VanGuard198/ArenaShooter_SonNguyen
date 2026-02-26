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
    // Tranh 2 vien dan vua ban ra va cham va dinh vao nhau loi logic physics
    // 1. Kiểm tra va chạm cơ bản
    if (!OtherActor || OtherActor == this || OtherActor == OwnerActor || OtherActor->IsA(ALoaiDanCuaTui::StaticClass()))
    {
        return;
    }
    {
        USkeletalMeshComponent* SkeletalMesh = OtherActor->FindComponentByClass<USkeletalMeshComponent>();
        UStaticMeshComponent* StaticMesh = OtherActor->FindComponentByClass<UStaticMeshComponent>();
        
        if (SkeletalMesh)
        {
            UMeshComponent* TargetMesh = Cast<UMeshComponent>(SkeletalMesh);

            // Kiểm tra an toàn xem Mesh có thực sự sở hữu Material nào không
            if (TargetMesh && TargetMesh->GetNumMaterials() > 0)
            {
                UMaterialInstanceDynamic* DynMat = TargetMesh->CreateDynamicMaterialInstance(0);

                if (DynMat)
                {
                    
                    //  Lấy màu gốc hiện tại của Material
                    FLinearColor OriginalColor;
                    DynMat->GetVectorParameterValue(FName("Paint Tint"), OriginalColor);
                    
                    // Thực hiện logic đổi màu đỏ:
                    DynMat->SetVectorParameterValue(TEXT("Paint Tint"), FLinearColor::Red);
                   
                    // Sau 1 khoang thoi gian thi quay ve mau goc
                    FTimerHandle LocalTimerHandle;
                    GetWorld()->GetTimerManager().SetTimer(
                        LocalTimerHandle,
                        [DynMat, OriginalColor]() // Capture OriginalColor vào lambda
                        {
                            if (DynMat)
                            {
                                DynMat->SetVectorParameterValue(FName("Paint Tint"), OriginalColor);
                            }
                        },
                        1.0f, false);
                }
            }
        }
        else if (StaticMesh)
        {
            UStaticMeshComponent* WallMesh = Cast<UStaticMeshComponent>(StaticMesh);
            if (!WallMesh) return;
            
            // Kiểm tra xem Mesh có Material nào không trước khi truy cập index 0
            if (WallMesh->GetNumMaterials() > 0)
            {
                UMaterialInstanceDynamic* DIMStatic = WallMesh->CreateDynamicMaterialInstance(0);

                if (DIMStatic)
                {
                    
                    //  Lấy màu gốc hiện tại của Material
                    FLinearColor OriginalColor;
                    DIMStatic->GetVectorParameterValue(FName("Base Color"), OriginalColor);
                    
                    
                    // Thực hiện logic của bạn ở đây, ví dụ đổi màu đỏ:
                    DIMStatic->SetVectorParameterValue(TEXT("Base Color"), FLinearColor::Blue);
                    
                    // Logic DIM doi mau khoang ... sau do quay ve mau trang
                    FTimerHandle LocalTimerHandle;
                    GetWorld()->GetTimerManager().SetTimer(
                        LocalTimerHandle,
                        [DIMStatic, OriginalColor]() // Capture OriginalColor vào lambda
                        {
                            if (DIMStatic)
                            {
                                DIMStatic->SetVectorParameterValue(FName("Base Color"), OriginalColor);
                            }
                        },
                        1.0f, false);
                }
            }
        }
    }
    
    // Destroy bullet after short delay
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ALoaiDanCuaTui::SelfDestroy, 0.1f, false);
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
