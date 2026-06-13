# InteractCore
![InteractCore](https://github.com/AliJimpa/InteractCore/blob/main/Resources/Icon128.png) 

`InteractCore` is a lightweight and extensible interaction framework designed for game development.

# Concept
`InteractCore` is built around a simple idea:
>"Everything that can be interacted with implements a common interface `(Interactable)` and communicates through a central interaction system.”

###### Structure
* `UInteractionComponent`: Is the main component for interaction (Input,Pivot)
    * `UDefaultInteraction`: Handel Optimization for raycast
        * `UCameraInteraction`:Handel Trace from camera for FPS game
        * `UProximityInteraction`: Handel trace and hovering with sphere for TPS game
        * `UMouseInteraction`: Handel trace from camera compatible with mouse for Isometric/TopDown game

* `UInteractableComponent`: Is the Sphere collider for Interaction
    * `UInteractableDetection`: Handel another sphere for detection befor hover
        * `UInteractablePoint`: Handel Indicator for Show Widget

* `IInteractable`: Base Interface for interaction indclude (hover,unhover,interact,ShouldHandleInput)

#### Interaction
```mermaid
classDiagram
    UInteractionComponent <|-- UDefaultInteraction
    UDefaultInteraction <|-- UCameraInteraction
    UDefaultInteraction <|-- UMouseInteraction
    UDefaultInteraction <|-- UProximityInteraction
    class UInteractionComponent{
        +EInteractionSearchMode DetectionMode
        +bool bInteractionInputBound
        +UInputAction InteractionInput
        +GetMode()
        +GetCurrentInteractable()
        GetPivot()
        +SetPivotToComponent()
        +SetPivotToTransform()
        +SetInteractionActive(bool bEnable)
    }
    class UDefaultInteraction{
        +bool DynamicInterval
        +EInteractionAdaptiveTickTrigger AdaptiveTickMode
        +float FastTickRate
        +float SlowTickRate
        +float RotationThreshold
        +float PositionThreshold
        +float VelocityThreshold
        +float MouseMovementThreshold
        +float ThresholdCustom
        +CustomAdaptiveTick()
        +CanHover()
        +CanInteract()
    }
    class UCameraInteraction{
        +EInteractionTraceType TraceType
        +FVector Offcet
        +float TraceDistance
        +TEnumAsByte<ECollisionChannel> TraceChannel
        +EInteractionTraceAxis TraceAxis
        +TArray<float> SphereFallback
        +bool bDrawDebugTrace
        +FColor DebugHitColor
        +FColor DebugNoHitColor
    }
    class UMouseInteraction{
        +float TraceDistance
        +TEnumAsByte<ECollisionChannel> TraceChannel
        +bool bDrawDebug
        +FColor DebugColor
    }
    class UProximityInteraction{
        +USphereComponent *DetectionSphere
        +FVector Offcet 
        +TArray<FHitResult> CandidateHits
        +bool UseInputForHovering
        +UInputAction *HoverInput
    }
```
#### Interactable
```mermaid
classDiagram
    UInteractableComponent <|-- UInteractableDetection
    UInteractableDetection <|-- UInteractablePoint
    class UInteractableComponent{
        +FOnInteractionEvent OnHoverBegin
        +FOnInteractionEvent OnHoverEnd
        +FOnInteractionEvent OnInteract
        +bool EnableInteraction
        +TEnumAsByte<ECollisionChannel> InteractChannel
        +EInteractionUsageMode InteractMode
        +float CooldowDuration
        +EInteractionInputMode InputMode
        +float HoldTimeThreshold
        +float DoubleTapInterval
        +bool IsHovered()
        +bool IsEnableInteraction()
        +bool HasInteracted()
        +bool IsInteractionPending()
        +void SetEnableInteraction(bool Enable)
        +void ResetInteraction()
        +bool IsAllowedInteraction()
        +bool IsAllowedInteraction_Implementation()
    }
    class UInteractableDetection{
        +FOnInteractionEvent OnZoneBegin
        +FOnInteractionEvent OnZoneEnd
        +USphereComponent *DetectionComponent
        +FVector DetectionOffset
        +float DetectionRadius
        +TEnumAsByte<ECollisionEnabled::Type> DetectionCollision
        +TEnumAsByte<ECollisionChannel> DetectionObjectType
        +FCollisionProfileName DetectionProfile
        +bool bFirstCheck
        +TEnumAsByte<ECollisionChannel> DetectionChannel
        +bool bDynamicRadius
        +float DistanceThreshold
        +FVector2D RadiusRange
        +bool bToggleRadius
        +float ToggleRadius
        +UInteractionComponent *GetCurrentDetected()
        +bool IsDetected()
        +void K2_ApplyZoneSettings()
        +void FirstCheck()
     }
    class UInteractablePoint{
        +TSubclassOf<UInteractionIndicatorWidget> IndicatorClass
        +EWidgetSpace WidgetSpace
        +EWidgetBlendMode WidgetBlendMode
        +FVector2D WidgetDrawSize
        +FVector WidgetOffset
        +bool bCheckLineOfSight
        +TEnumAsByte<ECollisionChannel> SightChannel
        +bool bShowDebugSight
        +void K2_ApplyWidgetSettings()
        +UInteractionIndicatorWidget *GetIndicator()
        +bool CanSeeDetectedObject() 
    }
```

## Features
1. Interaction include `DetectionMode` to define after raycast check implementation interface in component or actor or both
2. Interaction include Settting part that define positon and direction and raycast type
3. Optimazation part for performace manageing by other paramter for tracing
4. Interaction just compatible with Unreal Inhance Input Systme
5. Interactable include Dynamic Raduis that lerp sphere raduis by player distance 
6. Interactable include ToogleDetection that use for change detection raduis after player overlap
7. Interactable component incdule predefina some type of interaction type (tap,hover,doubletap)
8. Interactable after detection can sight to agent for hide indicator if agent was behind wall
9. Basic Indicator with all of type interaction define as widget in plugin

## Getting Started (FPS)
1. Add `UCameraInteraction` in your player and set interaction input asset
2. In any actor you want player interact with that you can do
    A. Implement `IInteractable` Interface
    B. Add `UInteractablePoint` component in your actor and add Your Widget or plugin default widget to show indicator 
after that you shouw use Events in your actor

> `IInteractable` include important function that need for interaction  
`ShouldHandleInput` this function get input and return bool if true agent can interact you can control input type in that like holding , double_tap ...
